lifsize=$(stat -c%s $1.lif)
if [ "$lifsize" -ge 1304576 ]; then
	echo 8 sector mode
	./hxcfe -uselayout:hp8sec.xml -finput:$1.lif -conv:HXC_HFE -foutput:$1.hfe
else
	./hxcfe -uselayout:hp.xml -finput:$1.lif -conv:HXC_HFE -foutput:$1.hfe
fi
