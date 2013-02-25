#!/usr/bin/env bash

[ $# -ne 1 ] && { echo "Usage $0 PROGRAM" >&2; exit 1; }

# set -v

PROGRAM=$1
BASENAME=`basename $PROGRAM .c`
echo $PROGRAM
../dce 1 1 < ${BASENAME}.3addr > ${BASENAME}.cfg
diff ${BASENAME}.cfg ${BASENAME}.ta.cfg
