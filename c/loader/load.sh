set -e
make
python ../../buildbinary.py bmbinary.s68 loader.bin
python ../../lifutils.py put ../../software.lif loader.bin SYSTEM_ C001
python ../../lifutils.py put ../../software.lif loader.bin PVTEST_ C001
(cd ../.. && ./lif2hfe.sh software)
cp ../../software.hfe d:/DSKA0004-Soft.HFE
echo DSKA0004.HFE