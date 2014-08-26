#!/usr/bin/env python3

board_size = 11

rings = [set() for _ in range(board_size // 2 + 1) ]

move_prio = [-1] * (board_size * board_size * 2)

def print_set_hex(s):
    val = 0
    for x in s:
        val = val | (1 << x)

    return "{:#018x}ul".format(val)

print("{")
for i in range(board_size **2):
    if i >= 64:
        print("{{{:#018x}ul,{:#018x}ul}},".format(1 << i-64, 0))
    else:
        print("{{{:#018x}ul,{:#018x}ul}},".format(0, 1 << i))

print("}")


print ("{")
for i in range(board_size ** 2):
    x = i // board_size
    y = i % board_size

    neighbours = set()

    if x > 0: neighbours.add((x-1) * board_size + y)
    if y > 0: neighbours.add(x * board_size + y - 1)
    if x < board_size - 1: neighbours.add((x+1) * board_size + y)
    if y < board_size - 1: neighbours.add(x * board_size + y + 1)

    rings[min(x, y, board_size - x -1, board_size - y - 1)].add(x*board_size + y)

    upper_set = {x-64 for x in neighbours if x >= 64}
    lower_set = {x for x in neighbours if x < 64}

    print("{{{}, {}}},".format(print_set_hex(upper_set), print_set_hex(lower_set)))



    # for (x = 0; x < BOARD_SIZE; ++x) {
    #     for (y = 0; y < BOARD_SIZE; ++y) {
    #         neighbors = 0;
    #
    #         if(x > 0) {
    #             neighbors |= ONE_128 << ((x-1) * BOARD_SIZE + y);
    #         }
    #
    #         if(y > 0) {
    #             neighbors |= ONE_128 << (x * BOARD_SIZE + (y-1));
    #         }
    #
    #         if(x < BOARD_SIZE - 1) {
    #             neighbors |= ONE_128 << ((x+1) * BOARD_SIZE + y);
    #         }
    #
    #         if(y < BOARD_SIZE - 1) {
    #             neighbors |= ONE_128 << (x * BOARD_SIZE + (y+1));
    #         }
    #
    #         NEIGHBORS[x* BOARD_SIZE + y] = neighbors;
    #     }
    # }

print ("}")

for i in range(board_size ** 2):
    x = i // board_size
    y = i % board_size

    ring1 = min(x, y, board_size - x -1, board_size - y - 1)
    if x < board_size - 1:
        x2 = x+1
        ring2 = min(x2, y, board_size - x2 -1, board_size - y - 1)
        move_prio[i * 2] = 20 - (ring1 + ring2)

    if y < board_size - 1:
        y2 = y+1
        ring2 = min(x, y2, board_size - x -1, board_size - y2 - 1)
        move_prio[i * 2 +1] = 20 - (ring1 + ring2)


print ("{")
for i in range(board_size // 2 + 1):
    upper_set = {x-64 for x in rings[i] if x >= 64}
    lower_set = {x for x in rings[i] if x < 64}
    print("{{{}, {}}},".format(print_set_hex(upper_set), print_set_hex(lower_set)))



print("}")

print("{")
print(", ".join(str(x) for x in move_prio))
print("}")

print(len(move_prio))