# Patch SYSTEM_ with an assembly patch.

import os
import sys
import struct

patch = bytearray()

JSR = b'\x4e\xb9'

checkedPos = False

with open(sys.argv[1],"r") as s:
    while True:
        line = s.readline()
        if not line:
            break
        if line.startswith("S2"):
            count = int(line[2:4],16)-4
            pos = int(line[4:4+6],16)
            
            if not checkedPos:
                PATCH_BASE = pos
                checkedPos = True
                
            data = bytes.fromhex(line[10:10+2*count])
            if pos >= PATCH_BASE:
                pos -= PATCH_BASE
                if pos > len(patch):
                    patch += (pos - len(patch)) * b'\x00'
                assert(pos == len(patch))
                patch += data
                
print("PATCH_BASE: %.8x" % PATCH_BASE)

with open(sys.argv[2],"rb") as f:
    header = f.read(0x2c)
    length,name,version,codeLength,codeStart = struct.unpack(">I26s6sII", header)
    assert codeStart == 0x984500
    endCode = codeLength + codeStart
    assert endCode <= PATCH_BASE
    code = f.read(codeLength)
    if codeLength + codeStart < PATCH_BASE:
        print("Inserting nulls.")
        code += (PATCH_BASE-endCode) * b'\x00'
    data = f.read()
    
for offset in range(len(patch)-8,-8,-8):
    v1,v2 = struct.unpack(">2I", patch[offset:offset+8])
    if v1 == 0:
        table = patch[offset+8:]
        patch = patch[:offset]
        dataNeeded = v2
        dataAddress = struct.unpack(">I", data[0:4])[0]
        endPatched = PATCH_BASE+len(patch)+dataNeeded
        print("Need %u bytes, have %u bytes." % (dataNeeded,dataAddress-endPatched))
        assert endPatched <= dataAddress
        offset = 0
        while offset + 8 <= len(table):
            print("Patch: jsr %.8x -> jsr %.8x" % struct.unpack(">2I", table[offset:offset+8]))
            code = code.replace(JSR + table[offset:offset+4], JSR + table[offset+4:offset+8], -1)
            offset += 8
        break
else:
    print("Patch table not found.")
    sys.exit(1)

codeLength += len(patch)
length += len(patch)

with open(sys.argv[3],"wb") as f:
    f.write(struct.pack(">I26s6sII", length,name,version,codeLength,codeStart))
    f.write(code)
    f.write(patch)
    f.write(data)
    