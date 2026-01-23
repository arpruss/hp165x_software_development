import subprocess 
import os
from tempfile import NamedTemporaryFile

def readHFE(filename):
    f = NamedTemporaryFile(delete=False)
    tempname = f.name
    f.close()
    pipe = subprocess.Popen(("hxcfe.exe", "-finput:"+filename, "-conv:RAW_LOADER", "-foutput:"+tempname),stdout=subprocess.DEVNULL,stderr=subprocess.DEVNULL)
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
    pipe = subprocess.Popen(("hxcfe.exe", "-uselayout:hp165x79.xml", "-finput:"+tempname, "-conv:HXC_HFE", "-foutput:"+filename),stdout=subprocess.DEVNULL,stderr=subprocess.DEVNULL)
    code = pipe.wait()
    os.unlink(tempname)
    assert code == 0
    