#!/bin/sh

for IND_TYPE in _CA _CC
do
	for USE_TRANS in _NO _TR
	do
		make clean
		make IND_TYPE=${IND_TYPE} USE_TRANS=${USE_TRANS}
	done
done
make clean
