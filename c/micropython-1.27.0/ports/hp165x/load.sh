make
python ../../../../buildbinary.py bmbinary.s68 python.bin
python ../../../../lifutils.py put python.hfe python.bin SYSTEM_ c001
python ../../../../lifutils.py put python.hfe test.py 1
if [ -e e:/HXCSDFE.CFG ] ; then
	drive=e:
else
	drive=d:
fi
cp python.hfe $drive/Python.hfe
echo $drive/Python.hfe
