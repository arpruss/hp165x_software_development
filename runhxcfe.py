import subprocess 
import os
import sys
import struct
from tempfile import NamedTemporaryFile

EXE = os.path.join(os.path.split(sys.argv[0]),"hxcfe.exe")

def readHFE(filename):
    f = NamedTemporaryFile(delete=False)
    tempname = f.name
    f.close()
    pipe = subprocess.Popen((EXE, "-finput:"+filename, "-conv:RAW_LOADER", "-foutput:"+tempname),stdout=subprocess.DEVNULL,stderr=subprocess.DEVNULL)
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
    if struct.unpack(">I",data[32:36]) == 4*8:
        xml = "hp8sec.xml"
    else:
        xml = "hp165x79.xml"
    print("Converting with %s.\n" % xml)
    pipe = subprocess.Popen((EXE, "-uselayout:"+directory+xml, "-finput:"+tempname, "-conv:HXC_HFE", "-foutput:"+filename),stdout=subprocess.DEVNULL,stderr=subprocess.DEVNULL)
    code = pipe.wait()
    os.unlink(tempname)
    assert code == 0
    