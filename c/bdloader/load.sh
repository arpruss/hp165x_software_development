set -e
make
python ../../lifutils.py put ../../big.hfe bmbinary.bin SYSTEM_ C001
python ../../lifutils.py put ../../big.hfe bmbinary.bin PVTEST_ C001
cp ../../big.hfe e:/DSKA0008.HFE
echo DSKA0008.HFE