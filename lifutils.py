import sys
import struct 
import subprocess 
import os
from pathlib import PurePath
from tempfile import NamedTemporaryFile


BLOCK_SIZE = 256
DIR_ENTRY_SIZE = 32
RESERVED_TRACK = 79
CHUNKING = True
CHUNK_FILLER = b'\xFF\xFF' + (BLOCK_SIZE-2)*b'\x00'
DIRECTORY = os.path.split(sys.argv[0])[0]
HXCFE = os.path.join(DIRECTORY,"hxcfe.exe")

def readHFE(filename):
    f = NamedTemporaryFile(delete=False)
    tempname = f.name
    f.close()
    pipe = subprocess.Popen((HXCFE, "-finput:"+filename, "-conv:RAW_LOADER", "-foutput:"+tempname),stdout=subprocess.DEVNULL,stderr=subprocess.DEVNULL)
    code = pipe.wait()
    with open(tempname, "rb") as f:
        data = f.read()
    os.unlink(tempname)
    assert code == 0
    return data

def writeHFE(filename,data):
    f = NamedTemporaryFile(delete=False,mode="wb")
    tempname = f.name
    f.write(data)
    f.close()
    pipe = subprocess.Popen((HXCFE, "-uselayout:"+os.path.join(DIRECTORY,"hp165x79.xml"), "-finput:"+tempname, "-conv:HXC_HFE", "-foutput:"+filename),stdout=subprocess.DEVNULL,stderr=subprocess.DEVNULL)
    code = pipe.wait()
    os.unlink(tempname)
    assert code == 0
def help():
    print("python lifutils.py create lifname.lif")
    print("python lifutils.py dir lifname.lif")
    print("python lifutils.py del lifname.lif FILE_TO_DELETE [more files]")
    print("python lifutils.py ren lifname.lif SOURCE_NAME DEST_NAME")
    print("python lifutils.py type lifname.lif FILE_TO_RETYPE FileType")
    print("python lifutils.py get lifname.lif FILE_TO_GET [host_filename]")
    print("python lifutils.py put lifname.lif [host_filename] FILE_TO_PUT FileType")
    print("python lifutils.py pack lifname.lif")
    print("With del or single-filename get you can use wildcards.")
    sys.exit(1)

if len(sys.argv) < 3:
    help()

def bytesToWord(b):
    return b[0] << 8 | b[1]

def wordToBytes(w):
    return bytes( ( w >> 8, w & 0xFF ) )

class DirEntry:
    def __init__(self, *args):
        if len(args) == 0:
            self.name = ""
            self.fileType = 1
            self.startBlock = 0
            self.blocks = 0
            self.year = 0x90
            self.month = 1
            self.day = 1
            self.hour = 1
            self.minute = 1
            self.second = 1
            self.misc = bytes.fromhex("8001534f544f")
            self.unchunkedFile = bytearray()
            self.chunkedFile = bytearray()
        else:
            name,self.fileType,self.startBlock,self.blocks,self.year,self.month,self.day,self.hour,self.minute,self.second,self.misc = struct.unpack(">10sHII6B6s", args[0])
            self.name = name.decode("cp437").strip()
            self.chunkedFile = diskData[self.startBlock*BLOCK_SIZE:(self.startBlock+self.blocks)*BLOCK_SIZE]
            self.unchunkedFile = unchunkFile(self.chunkedFile)
     
    def __str__(self):
        return "%s,%04X,%u,%u,%u,%02x/%02x/%02x %02x:%02x:%02x,%s" % (self.name,self.fileType,
                    self.startBlock,self.blocks,len(self.unchunkedFile),
                    self.year,self.month,self.day,self.hour,self.minute,self.second,
                    self.misc.hex())
                    
    def put(self, pos):
        diskData[dirStart * BLOCK_SIZE + pos * 32 : dirStart * BLOCK_SIZE + pos * DIR_ENTRY_SIZE + DIR_ENTRY_SIZE] = self.toBinary()
                    
    def toBinary(self):
        outName = (self.name + (10-len(self.name))*' ').encode("cp437")
        return struct.pack(">10sHII6B6s", outName,self.fileType,self.startBlock,self.blocks,self.year,self.month,self.day,self.hour,self.minute,self.second,self.misc)                       
        
def chunkFile(unchunked):
    if not CHUNKING:
        return unchunked
    chunked = bytearray()
    pos = 0
    while pos < len(unchunked):
        if pos + BLOCK_SIZE - 2 <= len(unchunked):
            size = BLOCK_SIZE - 2
        else:
            size = len(unchunked) - pos
        chunked += wordToBytes(size)
        chunked += unchunked[pos:pos+size]
        pos += size
    n = len(chunked) % BLOCK_SIZE
    if n % BLOCK_SIZE != 0:
        chunked += CHUNK_FILLER[:BLOCK_SIZE - n]
    return chunked
    
def unchunkFile(chunked):
    if not CHUNKING:
        return chunked
    unchunked = bytearray()
    pos = 0
    while pos < len(chunked):
        size = bytesToWord(chunked[pos:pos+2])
        unchunked += chunked[pos+2:pos+2+size]
        if size < BLOCK_SIZE - 2:
            break
        pos += 2+size
    return unchunked

rewrite = False

def fillFF(start,length):
    for j in range(length):
        diskData[start+j] = 0xFF

def delete(name):
    return retype(name, 0)
    
def rename(name, newName):
    name = name.upper()
    newName = newName.upper()
    for i in range(len(directory)):
        if name == directory[i][1].name.upper():
            directory[i][1].name = newName
            directory[i][1].put(directory[i][0])
            return True
    return False
    
def retype(name, newType):
    ret = False
    name = name.upper()
    for i in range(len(directory)):
        if PurePath(directory[i][1].name.upper()).match(name):
            n = directory[i][1].name
            directory[i][1].fileType = newType
            directory[i][1].put(directory[i][0])
            if newType == 0:
                print("Deleted %s" % n)
            else:
                print("Retyped %s" % n)
            ret = True
    return ret
    
def get(inFile,outFile):
    inFile = inFile.upper()
    for i in range(len(directory)):
        if inFile == directory[i][1].name.upper():
            with open(outFile, "wb") as outf:
                outf.write(directory[i][1].unchunkedFile)
            return True
    return False 

def getAll(inFile):
    ret = False
    inFile = inFile.upper()
    for i in range(len(directory)):
        if PurePath(directory[i][1].name.upper()).match(inFile):
            ret = get(directory[i][1].name,directory[i][1].name) or ret
            if ret:
                print("Got %s" % directory[i][1].name)
    return ret

def pack():
    filePos = dirStart + dirBlocks
    dirPos = 0
    
    newDirectory = bytearray()
    
    for _,entry in directory:
        diskData[filePos*BLOCK_SIZE:filePos*BLOCK_SIZE + len(entry.chunkedFile)] = entry.chunkedFile
        entry.name = entry.name.upper()
        entry.startBlock = filePos
        newDirectory += entry.toBinary()
        filePos += entry.blocks
    
    fillFF( filePos * BLOCK_SIZE, (totalBlocks - filePos) * BLOCK_SIZE)
    diskData[dirStart * BLOCK_SIZE : dirStart * BLOCK_SIZE + len(newDirectory)] = newDirectory
    fillFF( dirStart * BLOCK_SIZE + len(newDirectory), dirEntries * DIR_ENTRY_SIZE - len(newDirectory))
    
def put(inFile, outFile, fileType):
    outFile = outFile.upper()[:10]
    with open(inFile, "rb") as inf:
        data = chunkFile(inf.read())
    blocksNeeded = (len(data) + BLOCK_SIZE - 1) // BLOCK_SIZE
    data += (blocksNeeded * BLOCK_SIZE - len(data)) * b'\xFF'
    for i in range(len(directory)):
        entry = directory[i][1]
        if entry.name.upper() == outFile and entry.blocks == blocksNeeded:
            entry.fileType = fileType
            entry.name = outFile
            entry.put(directory[i][0])
            diskData[entry.startBlock * BLOCK_SIZE : (entry.startBlock + blocksNeeded) * BLOCK_SIZE] = data
            return True
    if delete(outFile):
        print("Deleted original")
        readDir(True)
    print("Packing")
    pack()
    readDir(True)
    if len(directory) + 1 > dirEntries:
        print("Out of space in directory")
        return False
    if lastBlock+blocksNeeded > totalBlocks:
        print("Needed %d, have %d blocks" % (blocksNeeded,totalBlocks-lastBlock))
        print("No space!")
        return False
    newEntry = DirEntry()
    newEntry.name = outFile
    newEntry.fileType = fileType
    newEntry.blocks = blocksNeeded
    newEntry.startBlock = lastBlock
    diskData[lastBlock*BLOCK_SIZE:lastBlock*BLOCK_SIZE+len(data)] = data
    diskData[dirStart*BLOCK_SIZE + len(directory) * DIR_ENTRY_SIZE : dirStart * BLOCK_SIZE + len(directory) * DIR_ENTRY_SIZE + DIR_ENTRY_SIZE] = newEntry.toBinary()
    return True

def readDir(quiet=False):                
    global directory,lastBlock
    directory = []
    startBlock = 0
    lastBlock = dirStart + dirBlocks
    for i in range(dirEntries):
        offset = dirStart * BLOCK_SIZE + i * DIR_ENTRY_SIZE
        if diskData[offset] != 0xFF:
            entry = DirEntry(diskData[offset:offset+DIR_ENTRY_SIZE])
            if entry.fileType:
                directory.append((i,entry))
                if entry.startBlock + entry.blocks > lastBlock:
                    lastBlock = entry.startBlock + entry.blocks
                if not quiet:
                    if entry.fileType == 0xC001:
                        comment = ''
                        try:
                            start = entry.startBlock
                            comment = "[" + entry.unchunkedFile[4:4+26].decode().strip() + " : " + entry.unchunkedFile[4+26:4+26+6].decode().strip() + "]"
                        except:
                            pass
                        print(i,entry,comment)                            
                    else:
                        print(i,entry)
    if not quiet:
        print("Last block",lastBlock)
        
def create(name):
    print("Creating "+name)
    header=bytes.fromhex("8000413136355820000000021000000000000012000000000000004F0000000200000014")
    tracks = 79
    sides = 2
    blocksPerTrack = 20
    totalBlocks = tracks * sides * blocksPerTrack
    with open(name,"wb") as outf:
        def writeReservedSector(data=b''):
            outf.write(data)
            outf.write(b'\xFF' * (1024-len(data)))
        outf.write(header)
        outf.write((BLOCK_SIZE-len(header)) * b'\x00')
        outf.write((BLOCK_SIZE*(totalBlocks-1)) * b'\xFF')
        for side in range(2):
            writeReservedSector(bytes.fromhex("E60000"))
            writeReservedSector(bytes.fromhex("5002880503010201A301A3E6321632"))
            writeReservedSector()
            writeReservedSector()
            writeReservedSector()

if sys.argv[1] == "--raw":
    CHUNKING = False
    sys.argv = sys.argv[:1] + sys.argv[2:]

cmd = sys.argv[1]

if cmd == "create":
    create(sys.argv[2])
    cmd = "dir"

if sys.argv[2].lower().endswith(".hfe"):
    print("Converting from hfe")
    diskData = bytearray(readHFE(sys.argv[2]))
else:    
    with open(sys.argv[2],"rb") as inf:
        diskData = bytearray(inf.read())

lifHeader, name, dirStart, lifId, dirBlocks, dirVersion, tracks, sides, blocksPerTrack = struct.unpack(">H6sIH2xIH2x3I", diskData[:36])
dirEntries = dirBlocks * BLOCK_SIZE // DIR_ENTRY_SIZE
if tracks == 0:
    print("assuming default geometry")
    tracks = 79
    sides = 2
    blocksPerTrack = 20
    diskData[24:36] = struct.pack(">3I",tracks,sides,blocksPerTrack)
if lifHeader != 0x8000:
    print("Not a valid lif file")
    sys.exit(2)
if lifId != 0x1000:
    print("Invalid lif ID %04x" % lifId)
    sys.exit(3)
if tracks > RESERVED_TRACK:
    print("fixing track info to take into account reserved track")
    tracks = RESERVED_TRACK
    diskData[24:36] = struct.pack(">3I",tracks,sides,blocksPerTrack)    
totalBlocks = tracks * sides * blocksPerTrack
print("Volume:",name.decode())
print("Directory start: %u\nDirectory length: %u blocks\nDirectory version: %u" % (dirStart,dirBlocks,dirVersion))
print("Tracks: %u\nSides: %u\nBlocks per track: %u\nTotal blocks: %u" % (tracks,sides,blocksPerTrack,totalBlocks))
readDir(cmd != "dir")
if cmd == "rm" or cmd == "del":
    for f in sys.argv[3:]:
        if delete(f):
            rewrite = True
        else:
            print("File %s not found" % sys.argv[3])
elif cmd == "ren":
    if rename(sys.argv[3], sys.argv[4]):
        rewrite = True
    else:
        print("File %s not found" % sys.argv[3])
elif cmd == "type":
    if retype(sys.argv[3], int(sys.argv[4],16)):
        rewrite = True
    else:
        print("File %s not found" % sys.argv[3])
elif cmd == "put":
    if len(sys.argv) >= 6:
        inFile = sys.argv[3]
        outFile = sys.argv[4]
        fileType = int(sys.argv[5],16)
    else:
        inFile = sys.argv[3]
        outFile = sys.argv[3]
        fileType = int(sys.argv[4],16)
    if put(inFile,outFile,fileType):
        rewrite = True
    else:
        print("Error putting %s -> %s" % (inFile,outFile))    
elif cmd == "get":
    ret = False
    if len(sys.argv) >= 5:
        ret = get(sys.argv[3], sys.argv[4])
    else:
        ret = getAll(sys.argv[3])
    if not ret:
        print("Error getting %s" % sys.argv[3])
elif cmd == "pack":
    pack()
    rewrite = True        
elif cmd != "dir":
    help()
 
if rewrite:
    print("rewriting")
    readDir()
    if sys.argv[2].lower().endswith(".hfe"):
        print("Converting to hfe")
        writeHFE(sys.argv[2], diskData)
    else:
        with open(sys.argv[2],"wb") as outf:
            outf.write(diskData)
        
        