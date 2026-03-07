#
# put an S-record file into a patch file that can be loaded onto an HP 165x logic analyzer as a _SYSTEM file
#

import os
import sys
import struct

patch = bytearray()
BASE  = 0xA09710
START = BASE
OLD_START_ADDRESS = 8 # relative to BASE
ROM_GET_KEY_PATCH = BASE+0xC
checkedPos = False

replaceFrom = bytes.fromhex("4EB90000EB38")
replaceTo = bytes.fromhex("4EB9%08X" % ROM_GET_KEY_PATCH)

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
                if pos > len(patch):
                    patch += (pos - len(patch)) * b'\x00'
                assert(pos == len(patch))
                patch += data
                
extraLength = len(patch)

with open(sys.argv[2],"rb") as f:
    header = f.read(0x2c)
    length,name,version,codeLength,base = struct.unpack(">I26s6sII", header)
    assert base == 0x984500
    assert codeLength + 0x984500 == BASE
    code = bytearray(f.read(codeLength))
    assert code[0:2] == b'\x4e\xf9'
    data = f.read()
    dataAddress = struct.unpack(">I", data[0:4])[0]
    print("Patch size: %d; space available: %d" % (len(patch), dataAddress-BASE))
    assert dataAddress >= BASE+len(patch)

patch[OLD_START_ADDRESS:OLD_START_ADDRESS+4] = code[2:6]
code[2:6] = struct.pack(">I", START)
assert replaceFrom in code
code = code.replace(replaceFrom,replaceTo,-1)
codeLength += extraLength
length += extraLength

with open(sys.argv[3],"wb") as f:
    f.write(struct.pack(">I26s6sII", length,name,version,codeLength,base))
    f.write(code)
    f.write(patch)
    f.write(data)
    