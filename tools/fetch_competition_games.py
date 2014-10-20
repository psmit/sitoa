import html
import re
import sys
from urllib.request import Request, urlopen

__author__ = 'psmit'

def get_log(base_url, gameid, cookie):
    request = Request(base_url + gameid, headers={'Cookie': cookie})
    response = urlopen(request)
    body = response.read().decode('utf-8')

    log = None
    for line in body.splitlines():
        if line.startswith("<b>Player messages:</b><br>"):
            log = html.unescape(line[len("<b>Player messages:</b><br>"):]).split("<br>")
        if line.startswith("<b>Checker messages:</b><br>You&nbsp;have&nbsp;been&nbsp;disqualified"):
            log = None

    if log:
        with open('../games/{}.log'.format(gameid), 'w') as out_f:
            out_f.write("\n".join(log))

def main(competition_url, cookie):
    request = Request(competition_url, headers={'Cookie': cookie})
    response = urlopen(request)
    body = response.read().decode('utf-8')

    regex = re.compile("showgame.php\?ga=([0-9]+)")

    for match in regex.findall(body):
        get_log('http://www.codecup.nl/showgame.php?ga=', match, cookie)


if __name__ == "__main__":
    main(sys.argv[1], sys.argv[2])