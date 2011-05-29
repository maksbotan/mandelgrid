
import struct
import zlib
import copy
import cPickle as pickle

MSG_HEADER = "MANDELBROT"
STR_HEADER = "BI"
HDR_SIZE = len(MSG_HEADER) + struct.calcsize("BI")

MSGT_ANNOUNCE = 1
MSGT_ANNOUNCE_RESP = 2
MSGT_REQUEST = 3
MSGT_RESPONSE = 4

class Message():

    __reserved = ("type", "content")

    def __init__(self, type, content={}, **kwargs):
        self.content = copy.copy(content)
        self.content.update(kwargs)
        self.type = type

    def send(self, socket):
        """
        Pack message and send to socket
        """

        packed_content = zlib.compress(pickle.dumps(self.content))
        content_length = len(packed_content)
        header = MSG_HEADER + struct.pack(STR_HEADER, self.type, content_length)
        message = header + packed_content

        socket.send(message)

    @staticmethod
    def load(socket):
        """
        Recieve message from socket and unpack it
        """

        header = socket.recv(HDR_SIZE)
        if header.startswith(MSG_HEADER):
            header = header[len(MSG_HEADER):]
        else:
            raise ValueError("Corrupted message (missing header)")
        type, length = struct.unpack(STR_HEADER, header)
        packed_content = socket.recv(length)
        content = pickle.loads(zlib.decompress(packed_content))

        return Message(type, content)

    def __getattr__(self, attr):
        return self.content[attr]

    def __setattr__(self, attr, value):
        if attr in self.__dict__ or attr in Message.__dict__ or attr in Message.__reserved:
            self.__dict__[attr] = value
            return
        self.content[attr] = value
