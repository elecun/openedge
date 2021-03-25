import zmq
import random
import time

context = zmq.Context()

# Socket with direct access to the sink: used to syncronize start of batch
sink = context.socket(zmq.PUSH)
sink.connect("tcp://*:5558")

# Initialize random number generator
random.seed()

while True:
    workload = random.randint(1, 100)
    sink.send(str(workload))
    
