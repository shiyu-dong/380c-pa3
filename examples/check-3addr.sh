#!/usr/bin/env bash

[ $# -ne 1 ] && { echo "Usage $0 PROGRAM" >&2; exit 1; }

# set -v

PROGRAM=$1
BASENAME=`basename $PROGRAM .c`
echo $PROGRAM
../dce -opt=none -backend=3addr < ${BASENAME}.3addr > ${BASENAME}.none.3addr
diff ${BASENAME}.none.3addr ${BASENAME}.3addr
