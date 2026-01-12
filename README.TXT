Here's a bunch of stuff to build 68K code running an HP165X logic analyzers.

I am using Windows with Cygwin. Work is needed to adapt this to pure Windows or Linux.

Most of this is an in-progress mess.

I am using a Gotek drive with hxc firmware. 

I assume you have have hxc floppy emulation software in ../floppy/hxc

Using EASy68K, you can assemble a 68K assembly file with an origin of 0x984500 to an S-record file
in asm/file.S68. Then you can create an .hfe virtual disc by running:
  ./builddisc.sh file
By default it also copies it to e:/DSKA0004.HFE. If you put that in your Gotek, you can either boot
your logic analyzer to it, or you can load it using the logic analyzer's IO menu as a "self test".

The notes are based on reverse engineering of both the SYSTEM_ and PVTEST_ programs.