
import socket
import interface


class Connection():
    def __init__(self, server=("127.0.0.1", 7200)):
        self.socket = socket.socket(type=socket.SOCK_DGRAM)
        self.socket.connect(server)
    
    def announce(self):
        """
        Notifies master server about client
        """
        self.socket.send(interface.build_announce())
        res = self.socket.recv(18)
        self.complete = interface.load_announce_resp(res)[0]
