import serial
import time

ser = serial.Serial("/dev/ttyUSB0", 115200)

data = open("image_bytes.bin", "rb").read()

for i in range(0, len(data), 480):
    while ser.read(1) != b'R':
        pass
    ser.write(data[i:i+480])

ser.close()