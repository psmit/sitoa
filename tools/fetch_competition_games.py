import re
import sys
from urllib.request import Request, urlopen

__author__ = 'psmit'

def get_log(base_url, id, cookie):
    request = Request(base_url + id, headers={'Cookie': cookie})
    response = urlopen(request)
    body = response.read().decode('utf-8')

    log = None
    for line in body.splitlines():
        if line.starswith("<b>Player messages:</b>"):
            log = 1
        if line.starswith("<b>Checker messages:</b><br>You&nbsp;have&nbsp;been&nbsp;disqualified"):
            log = None

    if log:
        pass

def main(competition_url, cookie):
    request = Request(competition_url, headers={'Cookie': cookie})
    response = urlopen(request)
    body = response.read().decode('utf-8')

    regex = re.compile("showgame.php\?ga=([0-9]+)")

    for match in regex.findall(body):
        get_log('http://www.codecup.nl/showgame.php?ga=', id, cookie)


if __name__ == "__main__":
    main(sys.argv[1], sys.argv[2])