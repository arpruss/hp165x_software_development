python buildbinary.py asm/$1.S68
python lifutils.py put software.lif binaryfile SYSTEM_ c001
python lifutils.py put software.lif binaryfile PVTEST_ c001
./lif2hfe.sh software
cp software.hfe e:/DSKA0004.HFE # if appropriate