import sys
file_contents = open(sys.argv[1]).readlines()
for i, line in reversed(file_contents):
    print("{} {}".format(line.strip(), i))
