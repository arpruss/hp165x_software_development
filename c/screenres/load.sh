set -e
make
prog=`basename $PWD`
(cd ../.. && ./s68todisc.sh $prog)