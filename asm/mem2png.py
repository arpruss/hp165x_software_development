import sys
from PIL import Image

with open(sys.argv[1], "rb") as f:
    binary = f.read()

img = Image.new(mode="RGB",size=(592,384))
for x in range(592):
    for y in range(384):
        address = (x // 4 * 2) + y * (592 // 2)
        mask = 8>>(x&3)
        if 0 == (mask & binary[address+1]):
            img.putpixel((x,y),(255,255,255))
img.save(sys.argv[1]+".png")            