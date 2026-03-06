lifsize=$(stat -c%s $1.lif)
if [ "$lifsize" -ge 2000000 ]; then
	./hxcfe -uselayout:hpbig.xml -finput:$1.lif -conv:HXC_HFE -foutput:$1.hfe
else
	./hxcfe -uselayout:hp165x79.xml -finput:$1.lif -conv:HXC_HFE -foutput:$1.hfe
fi
