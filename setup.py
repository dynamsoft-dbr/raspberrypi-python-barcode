from distutils.core import setup, Extension

module_dbr = Extension('dbr',
                        sources = ['dbr.c'], 
                        include_dirs=["/usr/lib/python2.7/dist-packages/numpy/core/include/numpy", "/home/pi/Desktop/dbr/include"],
                        library_dirs=['/home/pi/Desktop/dbr/lib'],
                        libraries=['DynamsoftBarcodeReader'])

setup (name = 'DynamsoftBarcodeReader',
        version = '1.0',
        description = 'Python barcode extension',
        ext_modules = [module_dbr])
