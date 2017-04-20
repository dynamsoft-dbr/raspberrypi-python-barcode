#include <Python.h>
#include "If_DBR.h"
#include "BarcodeFormat.h"
#include "BarcodeStructs.h"
#include "ErrorCode.h"
#include <ndarraytypes.h>

typedef unsigned long DWORD;
typedef long LONG;
typedef unsigned short WORD;

typedef struct tagBITMAPINFOHEADER {
  DWORD biSize;
  LONG biWidth;
  LONG biHeight;
  WORD biPlanes;
  WORD biBitCount;
  DWORD biCompression;
  DWORD biSizeImage;
  LONG biXPelsPerMeter;
  LONG biYPelsPerMeter;
  DWORD biClrUsed;
  DWORD biClrImportant;
} BITMAPINFOHEADER;

// Barcode format
const char * GetFormatStr(__int64 format)
{
    if (format == CODE_39)
        return "CODE_39";
    if (format == CODE_128)
        return "CODE_128";
    if (format == CODE_93)
        return "CODE_93";
    if (format == CODABAR)
        return "CODABAR";
    if (format == ITF)
        return "ITF";
    if (format == UPC_A)
        return "UPC_A";
    if (format == UPC_E)
        return "UPC_E";
    if (format == EAN_13)
        return "EAN_13";
    if (format == EAN_8)
        return "EAN_8";
    if (format == INDUSTRIAL_25)
        return "INDUSTRIAL_25";
    if (format == QR_CODE)
        return "QR_CODE";
    if (format == PDF417)
        return "PDF417";
    if (format == DATAMATRIX)
        return "DATAMATRIX";

    return "UNKNOWN";
}

/**
 * Initialize Dynamsoft Barcode Reader license. The method can only be called once. 
 * Invalid license is acceptable.
 */
static PyObject *
initLicense(PyObject *self, PyObject *args)
{
    char *license;
    if (!PyArg_ParseTuple(args, "s", &license)) {
    return NULL;
    }
    // printf("License: %s\n", license);
    int ret = DBR_InitLicense(license);
    return Py_BuildValue("i", ret);
}

/**
 * Decode barcode from a file 
 * 
 */
static PyObject *
decodeFile(PyObject *self, PyObject *args)
{
    char *pFileName;
	int iFormat;
    if (!PyArg_ParseTuple(args, "si", &pFileName, &iFormat)) {
        return NULL;
    }

    // Dynamsoft Barcode Reader initialization
    __int64 llFormat = iFormat;
    int iMaxCount = 0x7FFFFFFF;
    ReaderOptions ro = {0};
    pBarcodeResultArray pResults = NULL;
    ro.llBarcodeFormat = llFormat;
    ro.iMaxBarcodesNumPerPage = iMaxCount;

    // Barcode detection
    int ret = DBR_DecodeFile(pFileName, &ro, &pResults);
    // printf("DecodeFile ret: %d\n", ret);

    // Get results
    int count = pResults->iBarcodeCount;
    pBarcodeResult* ppBarcodes = pResults->ppBarcodes;
    pBarcodeResult tmp = NULL;
    PyObject* list = PyList_New(count); // The returned Python object
    PyObject* result = NULL;
    int i = 0;
    for (; i < count; i++)
    {
        tmp = ppBarcodes[i];
        result = PyString_FromString(tmp->pBarcodeData);
        PyList_SetItem(list, i, Py_BuildValue("iN", (int)tmp->llFormat, result)); // Add results to list
    }
    // release memory
    DBR_FreeBarcodeResults(&pResults);
    return list;
}

/**
 * Decode barcode from an image buffer. The supported data structure is DIB.
 * It is necessary to re-construct input data for barcode detection.
 */
static PyObject *
decodeBuffer(PyObject *self, PyObject *args)
{
    PyObject *o;
    int iFormat;
    if (!PyArg_ParseTuple(args, "Oi", &o, &iFormat))
        return NULL;

    PyObject *ao = PyObject_GetAttrString(o, "__array_struct__");
    PyObject *retval;

    if ((ao == NULL) || !PyCObject_Check(ao)) {
        PyErr_SetString(PyExc_TypeError, "object does not have array interface");
        return NULL;
    }

    PyArrayInterface *pai = (PyArrayInterface*)PyCObject_AsVoidPtr(ao);
    if (pai->two != 2) {
        PyErr_SetString(PyExc_TypeError, "object does not have array interface");
        Py_DECREF(ao);
        return NULL;
    }

    // Construct data with header info and image data 
    char *buffer = (char*)pai->data; // The address of image data
    int width = pai->shape[1];       // image width
    int height = pai->shape[0];      // image height
    int size = pai->strides[0] * pai->shape[0]; // image size = stride * height
    char *total = (char *)malloc(size + 40); // buffer size = image size + header size
    memset(total, 0, size + 40);
    BITMAPINFOHEADER bitmap_info = {40, width, height, 0, 24, 0, size, 0, 0, 0, 0};
    memcpy(total, &bitmap_info, 40);

    // Copy image data to buffer from bottom to top
    char *data = total + 40;
    int stride = pai->strides[0];
    int i = 1;
    for (; i <= height; i++) {
        memcpy(data, buffer + stride * (height - i), stride);
        data += stride;
    }

    // Dynamsoft Barcode Reader initialization
    __int64 llFormat = iFormat;
    int iMaxCount = 0x7FFFFFFF;
    ReaderOptions ro = {0};
    pBarcodeResultArray pResults = NULL;
    ro.llBarcodeFormat = llFormat;
    ro.iMaxBarcodesNumPerPage = iMaxCount;
    // printf("width: %d, height: %d, size:%d\n", width, height, size);
    int iRet = DBR_DecodeBuffer((unsigned char *)total, size + 40, &ro, &pResults);
    // printf("DBR_DecodeBuffer ret: %d\n", iRet);
    free(total); // Do not forget to release the constructed buffer 
    
    // Get results
    int count = pResults->iBarcodeCount;
    pBarcodeResult* ppBarcodes = pResults->ppBarcodes;
    pBarcodeResult tmp = NULL;
    retval = PyList_New(count); // The returned Python object
    PyObject* result = NULL;
    i = 0;
    for (; i < count; i++)
    {
        tmp = ppBarcodes[i];
        result = PyString_FromString(tmp->pBarcodeData);
        // printf("result: %s\n", tmp->pBarcodeData);
        PyList_SetItem(retval, i, Py_BuildValue("iN", (int)tmp->llFormat, result)); // Add results to list
    }
    // release memory
    DBR_FreeBarcodeResults(&pResults);

    Py_DECREF(ao);
    return retval;
}

static PyMethodDef Methods[] =
{
     {"initLicense", initLicense, METH_VARARGS, NULL},
     {"decodeFile", decodeFile, METH_VARARGS, NULL},
     {"decodeBuffer", decodeBuffer, METH_VARARGS, NULL},
     {NULL, NULL, 0, NULL}
};

PyMODINIT_FUNC
initdbr(void)
{
     (void) Py_InitModule("dbr", Methods);
}
