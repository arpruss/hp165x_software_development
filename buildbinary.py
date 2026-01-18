#
# put an S-record file into a binary file that can be loaded onto an HP 165x logic analyzer as a _SYSTEM file
#

import os
import sys
import struct

binary = bytearray()
BASE = 0x984500
MINIMUM_FILE_LENGTH = 0x400

with open(sys.argv[1],"r") as s:
    while True:
        line = s.readline()
        if not line:
            break
        if line.startswith("S2"):
            count = int(line[2:4],16)-4
            pos = int(line[4:4+6],16)
            data = bytes.fromhex(line[10:10+2*count])
            if pos >= BASE:
                pos -= BASE
                if pos > len(binary):
                    binary += (pos - len(binary)) * b'\x00'
                assert(pos == len(binary))
                binary += data
                
origLength = len(binary)
                
# I don't know what this padding is for!
binary = binary + 4 * b'\x00' 
                
with open("binaryfile","wb") as p:
    length = len(binary) + 8
    p.write( struct.pack(">I26s6sII", length, b"My Test Code".ljust(26, b' '), b"V00.00", origLength, 0x984500) )
    p.write(binary)
    
    
print("Binary was %d bytes" % origLength)
