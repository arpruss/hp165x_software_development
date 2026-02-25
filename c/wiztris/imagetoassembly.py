import sys
import os

WIDTH = 592
SCREEN = 0x620000
MOVEP = True

#
# given a text image, generate assembly code
# blanks are .
#

if sys.argv[1] == "--width":
    WIDTH = int(sys.argv[2])
    sys.argv = sys.argv[:1] + sys.argv[3:]
    
filename = sys.argv[1]
basename,_ = os.path.splitext(filename)
image = []
imageWidth = None
imageHeight = 0
with open(filename,"r") as f:
    for line in f:
        line = line.strip()
        if line:
            if imageWidth is None:
                imageWidth = len(line)
            else:
                assert(imageWidth == len(line))
            image.append(line)
            imageHeight += 1            
            
def makeImage(img,width,height,startOffset):
    code = "/* image %d */\n" % startOffset
    words = []
    dwords = []
    qwords = []
    
    def addValue(xpos,y,value):
        if value == 0:
            return
        pos = xpos + y * (WIDTH//2)
        if words and words[-1][0] == pos-2:
            dwords.append( (pos-2, words[-1][1] << 16 | value) )
            words.pop()
        else:
            words.append( ( pos, value ) )
    
    for y in range(height):
        value = 0
        mask = 8>>startOffset
        pos = 0
        for x in range(width):
            if img[y][x] != '.' and img[y][x] != ' ':
                value |= mask
            mask >>= 1
            if mask == 0:
                if value:
                    addValue(pos,y,value)
                    value = 0
                mask = 8
                pos += 2
        if value:
            addValue(pos,y,value)
         
    def swizzle(x,y):
        b0 = (x >> 16) & 0xF;
        b1 = (x) & 0xF;
        b2 = (y >> 16) & 0xF;
        b3 = (y) & 0xF;
        return b3 | b2<<8 | b1 << 16 | b0 << 24;
         
    if MOVEP:
        i = 0
        while i + 1 < len(dwords):
            if dwords[i+1][0] == dwords[i][0]+4:
                qwords.append((dwords[i][0] + 1, swizzle(dwords[i][1], dwords[i+1][1])))
                del dwords[i:i+2]
            else:
                i += 1
            
    def toDict(data):
        out = {}
        for datum in data:
            if datum[1] in out:
                out[datum[1]].append(datum[0])
            else:
                out[datum[1]] = [ datum[0], ]
        return out

    qwordDict = toDict(qwords)
    dwordDict = toDict(dwords)
    wordDict = toDict(words)
    def dest(p):
        if p == 0:
            return "(%a0)"
        else:
            return f"0x{p:04x}(%a0)"
    lastD0L = -1
    for dword in qwordDict:
        # movep takes a dword and spreads it out to write a qword
        positions = qwordDict[dword]
        lowWord = dword & 0xFFFF
        if len(positions) == 1 and lowWord not in wordDict:
            if positions[0] == 0:
                code += f"    movep.l #0x{dword:08x},{dest(positions[0])}\n"
        else:
            if dword < 128:
                code += f"    moveq #0x{dword:02x},%d0\n"
            else:
                code += f"    move.l #0x{dword:08x},%d0\n"
            for p in positions:
                code += f"    movep.l %d0,{dest(p)}\n"
            if lowWord in wordDict:
                for p in wordDict[lowWord]:
                    code += f"    move.w %d0,{dest(p)}\n"
                del wordDict[lowWord]                    
    for dword in dwordDict: 
        positions = dwordDict[dword]
        lowWord = dword & 0xFFFF
        if len(positions) == 1 and lowWord not in wordDict:
            if positions[0] == 0:
                code += f"    move.l #0x{dword:08x},{dest(positions[0])}\n"
        else:
            if dword < 128:
                code += f"    moveq #0x{dword:02x},%d0\n"
            else:
                code += f"    move.l #0x{dword:08x},%d0\n"
            for p in positions:
                code += f"    move.l %d0,{dest(p)}\n"
            if lowWord in wordDict:
                for p in wordDict[lowWord]:
                    code += f"    move.w %d0,{dest(p)}\n"
                del wordDict[lowWord]                    
    for word in wordDict:
        positions = wordDict[word]
        if len(positions) == 1:
            code += f"    move.w #0x{word:%04x},{dest(positions[0])}\n"
        else:
            if word < 128:
                code += f"    moveq #0x{word:02x},%d0\n" 
            else:
                code += f"    move.w #0x{word:04x},%d0\n" 
            for p in positions:
                code += f"    move.w %d0,{dest(p)}\n" 
    return code

print("""    .text
    .align 2
    .globl %s
    .type %s, @function 
%s:  /* void %s(uint16_t x,uint16_t y) */
""" % (basename,basename,basename,basename))


print("""
	movem.l %d0-%d2/%a0,-(%sp)
    move.w 26(%sp),%d0 /* y */
    mulu.w #(592/4),%d0     /* d0.w = y*(WIDTH/4) */
    move.w 22(%sp),%d1      /* x */
    move.w %d1,%d2
    lsr.w #2,%d1           /* d1 = x/4 */
    add.w %d1,%d0          /* d0 = y*(WIDTH/4) + x/4 */
    and.w #0xFFFF,%d0      
    add.l %d0,%d0          /* this may exceed 64k */
	move.l #0x600000,%a0
    add.l %d0,%a0          /* a0 = 0x600000 + (y*WIDTH/4+x/4)*2 */
    btst #0,%d2
    beq .even
    btst #1,%d2
    beq .image1
""")
print(makeImage(image,imageWidth,imageHeight,3))
print("""    movem.l (%sp)+,%d0-%d2/%a0
    rts
.even:
    btst #1,%d2
    beq .image0""")
print(makeImage(image,imageWidth,imageHeight,2))
print("""    movem.l (%sp)+,%d0-%d2/%a0
    rts
.image0:""")   
print(makeImage(image,imageWidth,imageHeight,0))
print("""    movem.l (%sp)+,%d0-%d2/%a0
    rts
.image1:""")   
print(makeImage(image,imageWidth,imageHeight,1))
print("""    movem.l (%sp)+,%d0-%d2/%a0
    rts""")
