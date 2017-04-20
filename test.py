import os.path
import dbr

types = {
        0x3FFL: "OneD",
        0x1L  : "CODE_39",
        0x2L  : "CODE_128",
        0x4L  : "CODE_93",
        0x8L  : "CODABAR",
        0x10L : "ITF",
        0x20L : "EAN_13",
        0x40L : "EAN_8",
        0x80L : "UPC_A",
        0x100L: "UPC_E",
        0x200L: "INDUSTRIAL_25",
        0x2000000L: "PDF417",
        0x8000000L: "DATAMATRIX",
        0x4000000L: "QR_CODE"
    }

def initLicense(license):
    dbr.initLicense(license)

def decodeFile(fileName):
    dbr.initLicense("B4E6216C77DC64BAB584DB7FF752B7D4")
    formats = 0x3FF | 0x2000000 | 0x8000000 | 0x4000000; # 1D, QRCODE, PDF417, DataMatrix
    results = dbr.decodeFile(fileName, formats)
    
    for result in results:
        print("barcode format: " + types[result[0]])
        print("barcode value: " + result[1])

if __name__ == "__main__":
    barcode_image = input("Enter the barcode file: ");
    if not os.path.isfile(barcode_image):
        print "It is not a valid file."
    else:
        decodeFile(barcode_image);
