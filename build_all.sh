#!/bin/sh

make clean
make IND_TYPE=_CC USE_TRANS=_TR
make clean
make IND_TYPE=_CC USE_TRANS=_NO
make clean
make IND_TYPE=_CA USE_TRANS=_TR
make clean
make IND_TYPE=_CA USE_TRANS=_NO
make clean
