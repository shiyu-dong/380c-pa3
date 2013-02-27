#!/usr/bin/env bash


C_SUBSET_COMPILER=../../csc/csc
THREE_ADDR_TO_C_TRANSLATOR=../../3addr-converter/convert.py

for PROGRAM in collatz.c gcd.c hanoifibfac.c loop.c mmm.c prime.c \
    regslarge.c struct.c sort.c sieve.c
do

  BASENAME=`basename $PROGRAM .c`
  ${C_SUBSET_COMPILER} $PROGRAM > ${BASENAME}.3addr
  ./check-dce.sh ${PROGRAM}
  ./check-cfg.sh ${PROGRAM}
  ./check-3addr.sh ${PROGRAM}
done
