make demos
for x in *.bin ; do 
    python ../../../lifutils.py put ../../../software.lif $x c001
done    
( cd ../../../ && ./lif2hfe.sh software && cp software.hfe d:/DSKA0004-Soft.hfe )
