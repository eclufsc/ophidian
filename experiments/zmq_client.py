import zmq
from pathlib import Path

class Client:
    def connect(self):
        print("Connecting to Ophidian server…")
        self.context = zmq.Context()
        self.socket = self.context.socket(zmq.REQ)
        self.socket.connect("tcp://localhost:5555")
        print("Connected")

    def readCircuit(self, path):
        circuit_name = Path(path).stem
        self.socket.send(str.encode("read_circuit "+circuit_name+" "+path))
        message = self.socket.recv()
        print("Check server log to see if the circuit was read.")

    def doILPRouting(self):
        self.socket.send(str.encode("ilp_route"))
        message = self.socket.recv()
        print(message.decode())


client = Client()
client.connect()
#client.sayHello()
client.readCircuit("/home/sheiny/workspace/ophidian/test/input_files/iccad2020/cases/case1.txt")
client.doILPRouting()
