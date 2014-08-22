#!/bin/bash

mkdir -p subm
> subm/sitoa.cpp
for f in $(cat include/order); do
cat include/$f >> subm/sitoa.cpp
echo "" >> subm/sitoa.cpp
done

cat sitoa.cpp >> subm/sitoa.cpp
mv subm/sitoa.cpp subm/sitoa.cpp.tmp
grep -v "include \"" subm/sitoa.cpp.tmp | grep -v "pragma" > subm/sitoa.cpp
g++ -std=c++0x -Wall -O2 -g -lm subm/sitoa.cpp -o subm/sitoa
