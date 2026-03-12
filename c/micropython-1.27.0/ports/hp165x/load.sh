make
python ../../../../buildbinary.py bmbinary.s68 python.bin
python ../../../../lifutils.py put python.hfe python.bin SYSTEM_ c001
python ../../../../lifutils.py put python.hfe test.py 1
cp python.hfe e:/DSKA0006.HFE