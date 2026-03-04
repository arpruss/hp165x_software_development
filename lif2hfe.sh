lifsize=$(stat -c%s $1.lif)
if [ "$lifsize" -ge 1304576 ]; then
	echo WARNING: 8 sector mode seems to work for reading, but not for writing
	./hxcfe -uselayout:hp8sec.xml -finput:$1.lif -conv:HXC_HFE -foutput:$1.hfe
else
	./hxcfe -uselayout:hp.xml -finput:$1.lif -conv:HXC_HFE -foutput:$1.hfe
fi
