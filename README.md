# Building Python Extension with DBR 5.2 for Raspberry Pi

## License
Get the [trial license](https://www.dynamsoft.com/CustomerPortal/Portal/Triallicense.aspx).

## Prerequisites
* [Dynamsoft Barcode Reader for Raspberry Pi][0]
* Python 2.7.0
* OpenCV 3.0.0
* Raspberry Pi 2 or 3
* USB webcam

You can also install Dynamsoft Barcode Reader via command line tool:

Add public key:
```
wget -O - http://labs.dynamsoft.com/debian/conf/dbr.gpg.key | sudo apt-key add -
```

Add source to /etc/apt/sources.list:
```
deb http://labs.dynamsoft.com/debian/ dbr main non-free
```

Install Dynamsoft Barcode Reader:
```
sudo apt-get update && install dbr
```

## Building and Installation
1. Create a symlink for **libDynamsoftBarcodeReader.so**:
    
    ```
    sudo ln –s <Your PATH>/libDynamsoftBarcodeReader.so /usr/lib/libDynamsoftBarcodeReader.so
    ```

    If you install dbr via command line tool, skip this step.

2. Install **Numpy**:

    ```
    pip install numpy
    ```

3. Build and install Python extension:
    
    ```
    # Python 2.7
    sudo python setup.py build install
    # Python 3.x
    sudo python3 setup.py build install
    ```

4. Run the test app:

    ```
    # Python 2.7
    python test.py
    # Python 3.x
    python3 test.py
    ```

## How to Run
1. Connect a webcam to Raspberry Pi 2 or 3.
2. Run **camera.py** or **camera_thread.py**: 

    ```
    python camera.py
    ```

![Raspberry Pi Barcode Scanner in Python](http://www.codepool.biz/wp-content/uploads/2016/11/rpi-python-webcam-small.png)

## Blog
[Raspberry Pi Barcode Scanner in Python][1]

[0]:http://www.dynamsoft.com/Downloads/Dynamic-Barcode-Reader-for-Raspberry-Pi-Download.aspx 
[1]:http://www.codepool.biz/raspberry-pi-barcode-scanner-python.html
