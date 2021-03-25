import sys
import time
import zmq

context = zmq.Context()

# Socket to receive messages on
receiver = context.socket(zmq.PULL)
receiver.connect("tcp://192.168.100.102:5555")

while True:
    print(receiver.recv())
