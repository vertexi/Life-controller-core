import zmq
from rapid_latex_ocr import LatexOCR

context = zmq.Context()
socket = context.socket(zmq.REP)
socket.bind("tcp://*:8848")

model = LatexOCR()

while True:
    print("waiting...")
    #  Wait for next request from client
    image = socket.recv()
    print("Received request!")
    res, elapse = model(image)
    socket.send_string(res)
    print(res)
    print(elapse)
