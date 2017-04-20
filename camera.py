import cv2
import dbr
import time

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

def get_time():
    localtime = time.localtime()
    capturetime = time.strftime("%Y%m%d%H%M%S", localtime)
    return capturetime

def read_barcode():

    vc = cv2.VideoCapture(0)
    vc.set(5, 30)  #set FPS
    vc.set(3, 320) #set width
    vc.set(4, 240) #set height

    if vc.isOpened(): # try to get the first frame
        dbr.initLicense("B4E6216C77DC64BAB584DB7FF752B7D4")
        rval, frame = vc.read()
    else:
        return
    
    windowName = "Barcode Reader"
    formats = 0x3FF | 0x2000000 | 0x8000000 | 0x4000000; # 1D, QRCODE, PDF417, DataMatrix

    while True:
        cv2.imshow(windowName, frame)
        rval, frame = vc.read();
        results = dbr.decodeBuffer(frame, formats)
        if (len(results) > 0):
            print(get_time())
            print("Total count: " + str(len(results)))
            for result in results:
                print("Type: " + types[result[0]])
                print("Value: " + result[1] + "\n")

        # 'ESC' for quit
        key = cv2.waitKey(20)
        if key == 27:
            dbr.destroy()
            break

    cv2.destroyWindow(windowName)

if __name__ == "__main__":
    print("OpenCV version: " + cv2.__version__)
    read_barcode()
