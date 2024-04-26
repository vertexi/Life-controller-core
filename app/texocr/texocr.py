import zmq
from rapid_latex_ocr import LatexOCR

context = zmq.Context()
socket = context.socket(zmq.PUB)
socket.bind("ws://*:8848")

model = LatexOCR()

img_path = "./test.png"
with open(img_path, "rb") as f:
    data = f.read()

    res, elapse = model(data)
    socket.send_string(res)
    socket.send_string(elapse)
    print(res)
    print(elapse)
