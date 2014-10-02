#!/usr/bin/env python
from __future__ import print_function
from subprocess import Popen, PIPE
from sys import stderr

def calculate(hashes):
    process = Popen(['./calculate_score'], stdin=PIPE, stderr=PIPE, stdout=PIPE)
    s = ""
    for hash in hashes:
        s += "{} {} {} {} {}\n".format(*hash)
    out, err = process.communicate(s)
    new_hashes = []
    for line in err.splitlines():
        if line.startswith("Randseed"): continue
        nh = tuple(t(s) for t,s in zip((int,str,str,int, int),line.split()))
        new_hashes.append(nh)

    dict_items = []

    for line in out.splitlines():
        h, m = line.split()
        print("{} {}".format(h,m))
        dict_items.append((h,m))
    return new_hashes, dict_items

def expand(hashes):
    process = Popen(['./expand_move_opponent'], stdin=PIPE, stderr=PIPE, stdout=PIPE)
    s = ""
    for hash in hashes:
        s += "{} {} {} {} {}\n".format(*hash)
    out, err = process.communicate(s)
    new_hashes = []
    for line in out.splitlines():
        nh = tuple(t(s) for t,s in zip((int,str,str,int, int),line.split()))
        new_hashes.append(nh)
    return new_hashes

def find_hashes(max_ply, hashes):
    current_ply = hashes[0][0]

    if current_ply > max_ply:
        return

    print("Ply: {}, Hashes: {}".format(current_ply, len(hashes)), file=stderr)

    calculated_hashes, _ = calculate(hashes)

    expanded_hashes = expand(calculated_hashes)
    expanded_hashes = list(set(expanded_hashes))

    find_hashes(max_ply, expanded_hashes)

def main():
    max_ply = 5
    depth = 1
    start_string = (0, "00aa8002aa000aa8002aa000aa8002aa", "00002aa800aaa002aa800aaa002aa800", depth, 0)

    expanded_hashes = expand([start_string])

    find_hashes(max_ply, [start_string])
    find_hashes(max_ply, expanded_hashes)

if __name__ == "__main__":
    main()