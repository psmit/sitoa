#!/bin/bash

mkdir -p subm

gcc -E -I include sitoa.c > subm/sitoa.c

gcc -Wall -O2 -g -lm subm/sitoa.c -o subm/sitoa