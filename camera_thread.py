import cv2
import dbr
import time
import threading
import Queue

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

q = Queue.Queue(1)

class BarcodeReaderThread (threading.Thread):
    def __init__(self, name, isRunning):
        threading.Thread.__init__(self)
        self.name = name
        self.isRunning = isRunning

    def run(self):
        global q
        formats = 0x3FF | 0x2000000 | 0x8000000 | 0x4000000; # 1D, QRCODE, PDF417, DataMatrix

        while self.isRunning:
            # Get a frame
            frame = q.get(True)
            results = dbr.decodeBuffer(frame, formats)
            q.task_done()

            if (len(results) > 0):
                print(get_time())
                print("Total count: " + str(len(results)))
                for result in results:
                    print("Type: " + types[result[0]])
                    print("Value: " + result[1] + "\n")

        print("Quit thread")

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

    # Create a thread for barcode detection
    barcodeReaderThread = BarcodeReaderThread("Barcode Reader Thread", True)
    barcodeReaderThread.start()

    global q
    while True:
        cv2.imshow(windowName, frame) # Render a frame on Window
        rval, frame = vc.read(); # Get a frame

        try:
            q.put_nowait(frame)
        except Queue.Full:
            try:
                q.get_nowait()
            except Queue.Empty:
                None

        # 'ESC' for quit
        key = cv2.waitKey(20)
        if key == 27:
            barcodeReaderThread.isRunning = False
            barcodeReaderThread.join()
            dbr.destroy()
            break

    cv2.destroyWindow(windowName)

if __name__ == "__main__":
    print "OpenCV version: " + cv2.__version__
    read_barcode()
