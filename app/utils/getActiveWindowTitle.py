import pyautogui
import time
import argparse
import zmq

context = zmq.Context()
socket = context.socket(zmq.PUB)
socket.bind("tcp://*:8848")

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
    parser.add_argument('-t', '--timeperiod', type=float, default=1, choices=[Range(0.0, 10.0)], help='sleep time in seconds')
    args = parser.parse_args()

    while True:
        windowTitle = pyautogui.getActiveWindowTitle()
        print(windowTitle)
        socket.send_string(windowTitle)
        time.sleep(args.timeperiod)
