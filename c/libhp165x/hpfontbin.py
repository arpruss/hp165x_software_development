import sys

def rol(x,n):
    return (x << n) | (x >> (32-n))

out = "uint8_t font[] = { ";
with open(sys.argv[1], "rb") as f:
    data = f.read()
bytesPerGlyph = len(data)//256
for i in range(256):
    if i % 8 == 0:
        out += "\n"
    glyph = data[i*bytesPerGlyph:(i+1)*bytesPerGlyph]
    if len(glyph) % 4:
        glyph += (4-len(glyph)%4) * b'\x00'
    for j in range(0,len(glyph),4):
        x1 = (glyph[j]&0xF) | (glyph[j]&0xF0)<<12
        x2 = (glyph[j+1]&0xF) | (glyph[j+1]&0xF0)<<12
        x3 = (glyph[j+2]&0xF) | (glyph[j+2]&0xF0)<<12
        x4 = (glyph[j+3]&0xF) | (glyph[j+3]&0xF0)<<12
        
        x = x1 | rol(x2,4) | rol(x3,8) | rol(x4,12)

        out += " 0x%02x,0x%02x,0x%02x,0x%02x," % (x>>24,(x>>16)&0xFF,(x>>8)&0xFF,x&0xFF)
out += " };\n";
print(out)
