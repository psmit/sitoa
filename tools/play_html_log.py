import os
import re
import sys

from subprocess import *

def main(d):
    for file in os.listdir(d):
        if not file.endswith(".html"):
            continue
        html_file = os.path.join(d, file)
        html_contents = open(html_file).read()

        pat = re.compile("MOVES = \[(('[A-Z][0-9]+-[A-Z][0-9]+'[,\s]*)*)\]")
        res = pat.search(html_contents)
        moves_string = res.group(1)
        moves = moves_string.strip().strip("'").split("', '")

        white = True if "White:&nbsp;&nbsp;</th><td class=kimborder>Peter " \
                        "Smit" in html_contents else False

        seed = re.search(";Rand&nbsp;init&nbsp;with&nbsp;seed&nbsp;(["
                         "a-f0-9]+)<br>", html_contents).group(1)

        if white:
            moves = ["Start"] + moves

        in_moves = [m for i,m in enumerate(moves) if i % 2 == 0] + ["Quit\n"]
        response_moves = [m for i,m in enumerate(moves) if i % 2 == 1]

        process = Popen(['build/sitoa', seed], stdin=PIPE, stderr=PIPE,
                        stdout=PIPE)
        (out, err) = process.communicate("\n".join(in_moves))

        m = out.split(None)

        if len(m) != len(response_moves):
            print("error in file " + html_file)

        for m1, m2 in zip(m, response_moves):
            if m1 != m2:
                print("error in file " + html_file)


        print(moves)
        print(white)
        print(seed)

        with open(file + ".log", 'w') as out_f:
            out_f.write(err)



if __name__ == "__main__":
    main(sys.argv[1])