#!/bin/sh

for IND_TYPE in _CA _CC
do
	for USE_TRANS in _NO _TR
	do
		for TUNER in RDA5807 TEA5767
		do
			make clean
			make IND_TYPE=${IND_TYPE} USE_TRANS=${USE_TRANS} TUNER=${TUNER}
		done
	done
done
make clean
