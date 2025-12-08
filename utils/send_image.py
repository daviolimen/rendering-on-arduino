import serial
import time

ser = serial.Serial("/dev/ttyUSB0", 115200)

data = open("image_bytes.bin", "rb").read()

for i in range(0, len(data), 480):
    ser.write(data[i:i+480])
    time.sleep(0.1)

ser.close()