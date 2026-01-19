if [ -f m68k_bare_metal/$1/bmbinary.s68 ] ; then
  python buildbinary.py m68k_bare_metal/$1/bmbinary.s68 $1.bin
else
  python buildbinary.py asm/$1.s68 $1.bin
fi || exit 1
if ! [ -f software.lif ] ; then
  python lifutils.py create software.lif
fi
if [ "$1" == "loader" ] ; then
	python lifutils.py put software.lif $1.bin SYSTEM_ c001
	python lifutils.py put software.lif $1.bin PVTEST_ c001
else
	python lifutils.py put software.lif $1.bin $1 c001
fi
./lif2hfe.sh software
cp software.hfe e:/DSKA0004.HFE # if appropriate
echo DSKA0004.HFE