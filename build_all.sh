#!/bin/sh

for PINOUT in PIN1 PIN2 PIN3
do
	make clean
    make PINOUT=${PINOUT}
done
make clean
