#!/bin/sh

for PINOUT in PIN1 PIN2 PIN3
do
	for IND_TYPE in CA CC
	do
		for USE_TRANS in NO TR
		do
			make clean
			make PINOUT=${PINOUT} IND_TYPE=${IND_TYPE} USE_TRANS=${USE_TRANS}
		done
	done
	make clean
    make PINOUT=${PINOUT} IND_TYPE=NIXIE
done
make clean
