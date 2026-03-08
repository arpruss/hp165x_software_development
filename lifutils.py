import sys
import struct 
import subprocess 
import os
from pathlib import PurePath
from tempfile import NamedTemporaryFile

GENERIC = False # set to True for use with devices other than HP 1652B/53B; may want to use RAW with it
BIGDISK_TRACKS = 254
SIDES = 2
BLOCK_SIZE = 256
DATA_TRACKS = 77
BLOCKS_PER_SECTOR = 4
SECTORS_PER_TRACK = 5
DIR_ENTRY_SIZE = 32
MAGIC_TRACK = 79 # set to 0 to disable
MAGIC_SIDE = 0
MAGIC_SECTOR = 1 # 0-based
MAGIC_BLOCKS = 4
CHUNKING = True
PACK = True
CHUNK_FILLER = b'\xFF\xFF' + (BLOCK_SIZE-2)*b'\x00'
CHUNK_FILLER_ODD = b'\x00\xFF\xFF' + (BLOCK_SIZE-3)*b'\x00'
DIRECTORY = os.path.split(sys.argv[0])[0]
HXCFE = os.path.join(DIRECTORY,"hxcfe.exe")
RESERVED_TYPE = 0xFEEF
RESERVED_MISC = b'\x80\x01RSVD'
RESERVED_NAME = "|RESERVED|"
BIGDISK = False

magicBlock = ((MAGIC_TRACK * SIDES + MAGIC_SIDE) * SECTORS_PER_TRACK + MAGIC_SECTOR) * BLOCKS_PER_SECTOR
magicData = bytes.fromhex("500288%02x03010201A301A3E6321632" % SECTORS_PER_TRACK)
magicData += (BLOCK_SIZE * MAGIC_BLOCKS - len(magicData)) * b'\xFF'

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
    if tracks > 80:
        xml = "hpbig.xml"
    else:
        xml = "hp165x79.xml"
    print("Converting with "+xml)
    pipe = subprocess.Popen((HXCFE, "-uselayout:"+os.path.join(DIRECTORY,xml), "-finput:"+tempname, "-conv:HXC_HFE", "-foutput:"+filename),stdout=subprocess.DEVNULL,stderr=subprocess.DEVNULL)
    code = pipe.wait()
    os.unlink(tempname)
    assert code == 0
def help():
    print("python lifutils.py create lifname.lif")
    print("python lifutils.py dir [-l] lifname.lif")
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
            if self.isReserved():
                self.unchunkedFile = self.chunkedFile
            else:
                self.unchunkedFile = unchunkFile(self.chunkedFile)
     
    def __str__(self):
        return "%s %04X %u [%u %u] %02x/%02x/%02x %02x:%02x:%02x,%s" % (self.name,self.fileType,
                    len(self.unchunkedFile),self.startBlock,self.blocks,
                    self.year,self.month,self.day,self.hour,self.minute,self.second,
                    self.misc.hex())
                    
    def format(self, verbose):
        if verbose: 
            return str(self)
        return "%-11s %04X %6u [%4u %4u]" % (self.name,self.fileType,len(self.unchunkedFile),
                    self.startBlock,self.blocks)
                    
    def isReserved(self):
        return ( self.fileType == RESERVED_TYPE and self.misc == RESERVED_MISC and
            self.name == RESERVED_NAME )
            
    def isSystem(self):
        return not GENERIC and self.fileType == 0xC001 and self.name == "SYSTEM_"
        
    @staticmethod
    def makeReserved():
        reservedEntry = DirEntry()
        reservedEntry.startBlock = magicBlock
        reservedEntry.blocks = MAGIC_BLOCKS
        reservedEntry.name = RESERVED_NAME
        reservedEntry.misc = RESERVED_MISC
        reservedEntry.fileType = RESERVED_TYPE    
        reservedEntry.chunkedFile = magicData
        return reservedEntry
                    
    def put(self, pos):
        diskData[dirStart * BLOCK_SIZE + pos * 32 : dirStart * BLOCK_SIZE + pos * DIR_ENTRY_SIZE + DIR_ENTRY_SIZE] = self.toBinary()
                    
    def toBinary(self):
        outName = (self.name + (10-len(self.name))*' ').encode("cp437")
        return struct.pack(">10sHII6B6s", outName,self.fileType,self.startBlock,self.blocks,self.year,self.month,self.day,self.hour,self.minute,self.second,self.misc)                       
        
def chunkFile(unchunked):
    if not CHUNKING:
        return unchunked + (256 - len(unchunked)%256) * '\x00'
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
        if n % 2 != 0:
            chunked += CHUNK_FILLER_ODD[:BLOCK_SIZE - n]
        else:
            chunked += CHUNK_FILLER[:BLOCK_SIZE - n]
    return chunked
    
def unchunkFile(chunked):
    if not CHUNKING:
        return chunked
    unchunked = bytearray()
    pos = 0
    while pos < len(chunked):
        size = bytesToWord(chunked[pos:pos+2])
        if len(chunked) == 28928:
            size = 254
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
    for i in range(len(directory)):
        if name == directory[i][1].name:
            directory[i][1].name = newName
            directory[i][1].put(directory[i][0])
            if BIGDISK and directory[i][1].isSystem() and i != 0:
                pack()
            return True
    return False
    
def match(name, pattern):
    if '?' in pattern or '*' in pattern:
        return PurePath(name).match(pattern)
    else:
        return name == pattern
    
def retype(name, newType):
    ret = False
    for i in range(len(directory)):
        if match(directory[i][1].name,name):
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
    for i in range(len(directory)):
        if inFile == directory[i][1].name:
            with open(outFile, "wb") as outf:
                outf.write(directory[i][1].unchunkedFile)
            return True
    return False 

def getAll(inFile):
    ret = False
    for i in range(len(directory)):
        if match(directory[i][1].name,inFile):
            ret = get(directory[i][1].name,directory[i][1].name) or ret
            if ret:
                print("Got %s" % directory[i][1].name)
    return ret
    
def disjoint(x,xcount,y,ycount):
    if x<=y:
        return x+xcount <= y
    else:
        return y+ycount <= x
        
def packFiles(d):
    global diskData
    
    fillFF( dirStart * BLOCK_SIZE, len(diskData) - dirStart * BLOCK_SIZE )
    if BIGDISK:
        for i in range(len(d)-1,-1,-1):
            if d[i].isReserved():
                del d[i]
        for i in range(1,len(d)):
            if d[i].isSystem():
                d = [ d[i], ] + d[:i] + d[i+1:]
                break
        block = dirStart + dirBlocks
        for i in range(len(d)):
            if block + d[i].blocks > magicBlock:
                e = DirEntry.makeReserved()
                d = d[:i] + [ e, ] + d[i:]
                break
            block += d[i].blocks
        else:
            d.append(DirEntry.makeReserved())
    if len(d) > dirEntries:
        print("Too many directory entries!")
        return False
    block = dirStart + dirBlocks
    for e in d:
        if BIGDISK and e.isReserved():
            block = e.startBlock + e.blocks
        else:
            e.startBlock = block
            block += e.blocks
        diskData[e.startBlock * BLOCK_SIZE : (e.startBlock + e.blocks) * BLOCK_SIZE] = e.chunkedFile
    if block > totalBlocks:
        print("Data does not fit!")
        return False
    binDir = b''.join((e.toBinary() for e in d))
    diskData[dirStart*BLOCK_SIZE : dirStart*BLOCK_SIZE + len(binDir)] = binDir

    return True

def pack():
    return packFiles([e for _,e in directory])
    
def put(inFile, outFile, fileType):
    outFile = outFile[:10]
    
    newEntry = DirEntry()
    newEntry.name = outFile
    newEntry.fileType = fileType
    
    with open(inFile, "rb") as inf:
        data = chunkFile(inf.read())
    newEntry.blocks = len(data) // BLOCK_SIZE
    newEntry.chunkedFile = data
    
    for i in range(len(directory)):
        entry = directory[i][1]
        if entry.name == outFile and entry.blocks == newEntry.blocks:
            entry.fileType = fileType
            entry.name = outFile
            entry.put(directory[i][0])
            diskData[entry.startBlock * BLOCK_SIZE : (entry.startBlock + newEntry.blocks) * BLOCK_SIZE] = data
            return True

    if delete(outFile):
        print("Deleted original")
        readDir(True)
        
    print("Packing directory and data")
    if not packFiles([d for _,d in directory] + [ newEntry, ]):
        return False
    readDir(True)
    return True

def readDir(quiet=False,verbose=False):                
    global directory,usedBlocks,largestSpace,dirEndOffset
    directory = []
    startBlock = 0
    usedBlocks = dirStart + dirBlocks
    largestSpace = 0
    previous = dirStart + dirBlocks
    dirEndOffset = previous * BLOCK_SIZE
    for i in range(dirEntries):
        offset = dirStart * BLOCK_SIZE + i * DIR_ENTRY_SIZE
        entry = DirEntry(diskData[offset:offset+DIR_ENTRY_SIZE])
        if entry.fileType == 0xFFFF:
            dirEndOffset = offset
            break
        if entry.fileType:
            if entry.startBlock - previous > largestSpace:
                largestSpace = entry.startBlock - previous
            directory.append((i,entry))
            if entry.startBlock + entry.blocks > usedBlocks:
                usedBlocks = entry.startBlock + entry.blocks
            previous = usedBlocks
            if not quiet:
                e = entry.format(verbose)
                if entry.fileType == 0xC001:
                    comment = ''
                    try:
                        start = entry.startBlock
                        comment = entry.unchunkedFile[4:4+26].decode().strip() 
                        if verbose:
                            comment += "\t" + entry.unchunkedFile[4+26:4+26+6].decode().strip()
                    except:
                        pass
                    print("%-3u"%i,e,comment)                            
                else:
                    print("%-3u"%i,e)
    if dirEndOffset < 0:
        dirEndOffset = dirStart + dirEntries * DIR_ENTRY_SIZE
    if totalBlocks - previous > largestSpace:
        largestSpace = totalBlocks - previous
    if not quiet:
        print("Used blocks:",usedBlocks)
        print("Largest space:",largestSpace)
        
def create(name):
    global diskData
    print("Creating "+name)
    blocksPerTrack = BLOCKS_PER_SECTOR * SECTORS_PER_TRACK
    tracks = DATA_TRACKS
    totalBlocks = tracks * SIDES * blocksPerTrack
    header=bytearray.fromhex("800041313635582000000002100000000000001200000000FFFFFFFF00000002FFFFFFFF")
    header[24:28] = struct.pack(">I", tracks)
    header[32:36] = struct.pack(">I", blocksPerTrack)
    if 0 < MAGIC_TRACK and MAGIC_TRACK < tracks:
        header[12:14] = struct.pack(">H", tracks)
    sides = 2
    diskData = bytearray()
    diskData += header
    diskData += (BLOCK_SIZE-len(header)) * b'\x00'
    diskData += (BLOCK_SIZE*(totalBlocks-1)) * b'\xFF'
    if MAGIC_TRACK >= 0 and MAGIC_TRACK < tracks:
        diskData[magicBlock * BLOCK_SIZE : (magicBlock + 1) * BLOCK_SIZE] = magicData
        diskData[2*BLOCK_SIZE:2*BLOCK_SIZE+DIR_ENTRY_SIZE] = DirEntry.makeReserved().toBinary()

def loadHeader():
    global lifHeader, name, dirStart, lifId, dirBlocks, dirVersion, tracks, sides, blocksPerTrack
    global dirEntries, BIGDISK, totalBlocks
    lifHeader, name, dirStart, lifId, dirBlocks, dirVersion, tracks, sides, blocksPerTrack = struct.unpack(">H6sIH2xIH2x3I", diskData[:36])
    dirEntries = dirBlocks * BLOCK_SIZE // DIR_ENTRY_SIZE
    if tracks == 0:
        print("assuming default geometry")
        tracks = DATA_TRACKS
        sides = 2
        blocksPerTrack = BLOCKS_PER_SECTOR * SECTORS_PER_TRACK
        diskData[24:36] = struct.pack(">3I",tracks,sides,blocksPerTrack)
    if lifHeader != 0x8000:
        print("Not a valid lif file")
        sys.exit(2)
    if GENERIC:
        if lifId != 0x1000:
            print("Invalid lif ID %04x" % lifId)
            sys.exit(3)
    else:
        if tracks == 80 or tracks == 79:
            tracks = DATA_TRACKS
        if lifId == BIGDISK_TRACKS:
            BIGDISK = True
            tracks = BIGDISK_TRACKS
            print("Big disk mode")
    totalBlocks = tracks * sides * blocksPerTrack - 1
    #if totalBlocks > DATA_TRACKS * sides * blocksPerTrack - 1:
    #    totalBlocks = DATA_TRACKS * sides * blocksPerTrack - 1
    print("Volume:",name.decode())
    print("Directory start: %u\nDirectory length: %u blocks\nDirectory version: %u" % (dirStart,dirBlocks,dirVersion))
    print("Tracks: %u\nSides: %u\nBlocks per track: %u\nTotal blocks: %u" % (tracks,sides,blocksPerTrack,totalBlocks))
    readDir(cmd != "dir", verbose="-l" in options)

while sys.argv[1].startswith("--"):
    if sys.argv[1] == "--raw":
        CHUNKING = False
        sys.argv = sys.argv[:1] + sys.argv[2:]
    elif sys.argv[1] == "--generic":
        GENERIC = False
        MAGIC_TRACK = -1
        sys.argv = sys.argv[:1] + sys.argv[2:]
    elif sys.argv[1] == "--nopack":
        PACK = False
        sys.argv = sys.argv[:1] + sys.argv[2:]
    elif sys.argv[1] == "--bigdisk":
        BIGDISK = True
        DATA_TRACKS = BIGDISK_TRACKS
        sys.argv = sys.argv[:1] + sys.argv[2:]
    elif sys.argv[1] == "--tracks":
        DATA_TRACKS = int(sys.argv[2])
        sys.argv = sys.argv[:1] + sys.argv[3:]
    else:
        assert False

cmd = sys.argv[1]

options = []
while sys.argv[2][0] == "-":
    options.append(sys.argv[2])
    if sys.argv[2] == "-" or sys.argv[2] == "--":
        break
    sys.argv = sys.argv[:2] + sys.argv[3:]

if cmd == "create":
    create(sys.argv[2])
    loadHeader()
    if sys.argv[2].lower().endswith(".hfe"):
        print("Converting to hfe")
        writeHFE(sys.argv[2], diskData)
    else:
        with open(sys.argv[2],"wb") as outf:
            outf.write(diskData)
    readDir()
    sys.exit(0)

if sys.argv[2].lower().endswith(".hfe"):
    print("Converting from hfe")
    diskData = bytearray(readHFE(sys.argv[2]))
else:    
    with open(sys.argv[2],"rb") as inf:
        diskData = bytearray(inf.read())

    
loadHeader()    
    
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
        outFile = os.path.basename(sys.argv[3])
        if len(sys.argv) >= 5:
            fileType = int(sys.argv[4],16)
        else:
            print("Assuming file type 0001")
            fileType = 1
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
        
        