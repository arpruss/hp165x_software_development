set -e
make -f Makefile
prog=`basename $PWD`
(cd ../.. && ./s68todisc.sh $prog)