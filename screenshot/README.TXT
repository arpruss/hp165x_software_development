This patch allows you to generate a screenshot by pressing the sequence:
 [Don't Care] [Clear Entry] [Don't Care] [Clear]
Make sure you switch to a disk with sufficient space. I don't know how
to force a refresh in the software, so if you switch disks, go to the
I/O Disk Operations before doing a screenshot to force a refresh.

The screenshots take up 28683 bytes.

To install the screenshot mod, first copy SYSTEM_ from your system disk with lifutils.py:
   python ../lifutils.py systemdisk.lif get SYSTEM_
Make sure it's 658428 bytes long. Otherwise, you will have to modify screendump.x68.
Run:
   python patch.py screendump.S68 SYSTEM_ patched
This makes a patched OS file. Make a backup copy of your systemdisk.lif and do:
   python ../lifutils put systemdisk.lif patched SYSTEM_ c001
Then convert systemdisk.lif to hfe and boot from it.