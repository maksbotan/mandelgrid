
import struct
from socket import gethostname

"""
Binary message structures:
"Announce" packets:
struct announce {
    string hostname;
}

struct announce_resp {
    char completeness;
}

struct request {
    double x1, x2;
    double y1, y2;
    unsigned long quality;
    double xstep, ystep;
}

struct response {
    unsinged long long len;
    string res; /* actually a two dimensional array of ulonglongs
}
"""

HDR_ANNOUNCE="MAN_ANNOUNCE"
HDR_ANNOUNCE_RESP = "MAN_ANNOUNCE_RESP"
HDR_REQUEST = "MAN_REQUEST"
HDR_RESPONSE = "MAN_RESPONSE"
PK_REQUEST = r"ddddLdd"
PK_RESPONSE = r"Qs"

class ProtocolError(Exception):
    pass

def build_announce():
    hostname = gethostname()
    return HDR_ANNOUNCE + hostname

def load_announce(buf):
    if not buf.startswith(HDR_ANNOUNCE):
        raise ProtocolError
    return buf[len(HDR_ANNOUNCE):]

def build_announce_resp(completeness):
    return HDR_ANNOUNCE_RESP + struct.pack("b", completeness)

def load_announce_resp(buf):
    print buf
    if not buf.startswith(HDR_ANNOUNCE_RESP):
        raise ProtocolError
    return struct.unpack("b", buf[len(HDR_ANNOUNCE_RESP):])

def build_request(x1, x2, y1, y2, quality, xstep, ystep):
    return HDR_REQUEST + struct.pack(PK_REQUEST, x1, x2, y1, y2, quality, xstep, ystep)

def load_request(buf):
    if not buf.startswith(HDR_REQUEST):
        raise ProtocolError
    return dict(zip(
        ("x1", "x2", "y1", "y2", "quality", "xstep", "ystep"),
        struct.unpack(PK_REQUEST, buf[len(HDR_REQUEST):])))

def build_response(result):
    res = []
    for i in result:
        res += i
    return HDR_RESPONSE + struct.pack("Q"*len(res), *res)

def load_response(buf, xres, yres):
    if not buf.startswith(HDR_RESPONSE):
        raise ProtocolError
    l = list(struct.unpack("Q"*(xres*yres), buf[len(HDR_RESPONSE):]))
    res = []
    for i in xrange(yres):
        res.append(l[:xres])
        del l[:xres]
    return res
