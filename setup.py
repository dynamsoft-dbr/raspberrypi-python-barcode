from distutils.core import setup, Extension
import os, numpy

numpy_include = os.path.join(os.path.dirname(numpy.__file__), "core", "include", "numpy")
print(numpy_include)
module_dbr = Extension('dbr',
                        sources = ['dbr.c'], 
                        include_dirs=[numpy_include],
                        libraries=['DynamsoftBarcodeReader'])

setup (name = 'DynamsoftBarcodeReader',
        version = '1.0',
        description = 'Python barcode extension',
        ext_modules = [module_dbr])
