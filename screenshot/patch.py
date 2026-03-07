#
# put an S-record file into a binary file that can be loaded onto an HP 165x logic analyzer as a _SYSTEM file
#

import os
import sys
import struct

binary = bytearray()
BASE = 0xA09710
checkedPos = False

replaceFrom = bytes.fromhex("4EB90000EB38")
replaceTo = bytes.fromhex("4EB9%08X" % BASE)

with open(sys.argv[1],"r") as s:
    while True:
        line = s.readline()
        if not line:
            break
        if line.startswith("S2"):
            count = int(line[2:4],16)-4
            pos = int(line[4:4+6],16)
            
            if not checkedPos:
                if pos != BASE:
                    print("Base should be %x but is %x" % (BASE,pos))
                    sys.exit(1)
                else:
                    checkedPos = True
                
            data = bytes.fromhex(line[10:10+2*count])
            if pos >= BASE:
                pos -= BASE
                if pos > len(binary):
                    binary += (pos - len(binary)) * b'\x00'
                assert(pos == len(binary))
                binary += data
                
extraLength = len(binary)

with open(sys.argv[2],"rb") as f:
    header = f.read(0x2c)
    length,name,version,codeLength,base = struct.unpack(">I26s6sII", header)
    assert base == 0x984500
    assert codeLength + 0x984500 == BASE
    code = f.read(codeLength)
    data = f.read()

newcode = code.replace(replaceFrom,replaceTo,-1)
assert code != newcode
code = newcode
codeLength += extraLength
length += extraLength

with open(sys.argv[3],"wb") as f:
    f.write(struct.pack(">I26s6sII", length,name,version,codeLength,base))
    f.write(code)
    f.write(binary)
    f.write(data)
    