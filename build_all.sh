#!/bin/sh

for PINOUT in _PIN1 _PIN2
do
	for IND_TYPE in _CA _CC
	do
		for USE_TRANS in _NO _TR
		do
			make clean
			make PINOUT=${PINOUT} IND_TYPE=${IND_TYPE} USE_TRANS=${USE_TRANS}
		done
	done
	make clean
    make PINOUT=${PINOUT} IND_TYPE=_NIXIE
done
make clean
