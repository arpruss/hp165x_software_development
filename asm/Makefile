.SUFFIXES: .x68 .S68

all: dumper.S68 dumperkey.S68

.x68.S68:
	../../vasm/vbcc/bin/vasmm68k_mot -Fsrec $< -o $@
    
