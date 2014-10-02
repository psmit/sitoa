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

    real_items = []
    cur_val = None
    for h,v in items:
        if v != cur_val:
            cur_val = v
            real_items.append((h,v))

    print(len(real_items))


    print("#pragma once")
    print()
    print("""
#define CODEBOOK_MAXPLY 4

    """)



if __name__ == "__main__":
    main()