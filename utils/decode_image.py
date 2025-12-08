# Simple python script to convert jpg image to binary using rgb565 color scheme

from PIL import Image
import struct

img = Image.open("test_image.jpg").convert("RGB")
img = img.transpose(Image.FLIP_LEFT_RIGHT)
img = img.resize((240, 320))
image_bytes = bytearray()

for y in range(320):
    for x in range(240):
        r, g, b = img.getpixel((x, y))

        r5 = (r >> 3) & 0x1f
        g6 = (g >> 2) & 0x3f
        b5 = (b >> 3) & 0x1f

        value = (r5 << 11) | (g6 << 5) | b5

        image_bytes += struct.pack(">H", value)

with open("image_bytes.bin", "wb") as f:
    f.write(image_bytes)
