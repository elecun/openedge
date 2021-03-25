import zmq
import time

context = zmq.Context()
socket = context.socket(zmq.PUB)
socket.connect("tcp://*:5556")

# Sleep a bit of time until the connection is properly established,
# otherwise some messages may be lost.
# http://stackoverflow.com/questions/7470472/lost-messages-on-zeromq-pub-sub
time.sleep(1)

for i in range(100):
    message = "foo {}".format(i)
    print(message)
    socket.send_string(message)
    time.sleep(1)
