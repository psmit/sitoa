#!/usr/bin/env python
from __future__ import print_function
import fileinput

def main():
    num_chars = 8
    items = []
    for line in fileinput.input():
        h, v = line.split()
        h = int(h,0)
        items.append((h,v))

    items = sorted(items, key=lambda x: x[0])





if __name__ == "__main__":
    main()