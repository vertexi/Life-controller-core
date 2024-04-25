from getActiveWindowTitle import getActiveWindowTitle
from getCurrentMedia import getCurrentMedia

import time
import argparse
import zmq

context = zmq.Context()
socket = context.socket(zmq.PUB)
socket.bind("ws://*:8848")

class Range(object):
    def __init__(self, start, end):
        self.start = start
        self.end = end

    def __eq__(self, other):
        return self.start <= other <= self.end

    def __contains__(self, item):
        return self.__eq__(item)

    def __iter__(self):
        yield self

    def __str__(self):
        return '[{0},{1}]'.format(self.start, self.end)

if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        prog='PROG',
        formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument('-t', '--timeperiod', type=float, default=5, choices=[Range(0.0, 60.0)], help='sleep time in seconds')
    parser.add_argument('-v', '--verbose', action='store_true', default=False, help='increase output verbosity')
    args = parser.parse_args()

    while True:
        info = f"#WINDOW: {getActiveWindowTitle()}\n#MEDIA: {getCurrentMedia()}"
        if args.verbose:
            print(info)
        socket.send_string(info)
        time.sleep(args.timeperiod)
