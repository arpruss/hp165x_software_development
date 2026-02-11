from qreader import QReader
import sys
import cv2
import base64
from pyzbar import pyzbar

qreader = QReader(model_size='n')

memory = bytearray()
filled = bytearray()

def decodeImage(imgData):
    decoded = False and pyzbar.decode(imgData)
    if decoded:
        return decoded[0].data.decode()
    decoded = qreader.detect_and_decode(image=imgData)
    if decoded:
        return decoded[0]
    return False

def decode(img):
    global memory,filled
    data = decodeImage(cv2.imread(img))
    if not data or len(data)<16:
        print("Decoding %s failed" % img)
        return False
    position = int(data[:8],16)
    length = int(data[9:13],16)
    try:
        binary = base64.b64decode(data[14:])
    except:
        print("Error base64 decoding %s" % img)
        return False
    if len(binary) != length:
        print("length mismatch: supposed to be %d but have %d" % (length,len(binary)))
        return False
    
    print("decoded %s at %08x of length %04x" % (img,position,length))
    if position+length > len(memory):
        pad = (position+length-len(memory))*b'\x00'
        memory += pad
        filled += pad
    memory[position:position+length] = binary
    filled[position:position+length] = length * b'\x01'
    return True    

def ranges():
    pos = 0
    start = -1
    while pos < len(filled):
        if filled[pos]:
            if start < 0:
                start = pos
        else:
            if 0 <= start:
                yield(start,pos)
                start = -1
        pos += 1
    if 0 <= start:
        yield(start,pos)
    
    
for f in sys.argv[1:]:
    decode(f)

for start,stop in ranges():
    print("%08x-%08x"%(start,stop))
    
with open("memory.bin","wb") as m:
    m.write(memory)