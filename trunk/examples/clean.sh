#!/usr/bin/env bash

for PROGRAM in collatz.c gcd.c hanoifibfac.c loop.c mmm.c prime.c \
    regslarge.c struct.c sort.c sieve.c
do
  BASENAME=`basename $PROGRAM .c`
  rm -rf ${BASENAME}.3addr ${BASENAME}.3addr.cfg ${BASENAME}.cfg
done
