#!/bin/sh

gcc "$@" -fopenmp -Wall -o test test.c trace.c trace.ld
