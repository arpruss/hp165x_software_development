if [ -f m68k_bare_metal/$1/bmbinary.s68 ] ; then
  python buildbinary.py m68k_bare_metal/$1/bmbinary.s68
else
  python buildbinary.py asm/$1.s68
fi || exit 1
python lifutils.py put software.lif binaryfile SYSTEM_ c001
python lifutils.py put software.lif binaryfile PVTEST_ c001
./lif2hfe.sh software
cp software.hfe e:/DSKA0004.HFE # if appropriate