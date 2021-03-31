import zmq

context = zmq.Context()
socket = context.socket(zmq.SUB)
# accept all topics (prefixed) - default is none
socket.setsockopt_string(zmq.SUBSCRIBE, "")
socket.connect("tcp://192.168.100.102:5661")

while True:
    message = socket.recv_string()
    print(message)
