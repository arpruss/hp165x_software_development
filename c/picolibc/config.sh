meson setup build-68000 \
    --cross-file scripts/cross-m68k-elf.txt \
    -Dformat-default=integer \
    -Dthread-local-storage=false \
	-Dpicocrt=false \
	-Dmultilib=false \
    -Dtests=false \
	-Dio-long-double=false \
	-Dfast-bufio=true \
	--wipe
	
ninja -C build-68000
DESTDIR=$(pwd)/../picolibc-68000 ninja -C build-68000 install	
