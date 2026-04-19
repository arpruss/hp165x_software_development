python lifutils.py --bigdisk create big.lif
#python lifutils.py put big.lif screenshot/patched c001
python lifutils.py put big.lif c/loader/bmbinary.bin SYSTEM_ c001
python lifutils.py put big.lif dumper.bin c001
python lifutils.py put big.lif c/micropython-1.27.0/ports/hp165x/python.bin c001
python lifutils.py put big.lif screenshot/patched system c001
python lifutils.py put big.lif c/wiztris/wiztris.bin Wiztris c001
python lifutils.py put big.lif c/loader/bmbinary.bin PVTEST_ c001
#python lifutils.py del big.lif dumper.bin
./lif2hfe.sh big
cp big.hfe e:/dska0008.hfe