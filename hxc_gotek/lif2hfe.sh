#python lif2xml.py $1 && ./hxcfe -finput:$1.xml -conv:HXC_HFE -foutput:$1.hfe
./hxcfe -uselayout:hp.xml -finput:$1.lif -conv:HXC_HFE -foutput:$1.hfe
