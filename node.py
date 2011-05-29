
import socket, select, argparse
import mandel, interface

parser = argparse.ArgumentParser()
parser.add_argument("-s", "--server",
                    default="localhost",
                    type=socket.gethostbyname,
                    help="Master server to connect to")
parser.add_argument("-p", "--port",
                    default=7200,
                    type=int,
                    help="Port of master server")
args = parser.parse_args()

s = socket.socket()
s.connect((args.server, args.port))

announce = interface.Message(interface.MSGT_ANNOUNCE, hostname=socket.gethostname())
announce.send(s)

state = 0 #announce sent

while 1:
    select.select([s], [], [])
    try:
        msg = interface.Message.load(s)
    except ValueError:
        print "Server stopped"
        break
    if msg.type == interface.MSGT_ANNOUNCE_RESP:
        state = 1 #anounce recieved
        print "Connected to {0}:{1}".format(args.server, args.port)
        print msg.content
    if msg.type == interface.MSGT_REQUEST:
        if state != 1:
            continue
        bounds, scale, quality = msg.block, msg.scale, msg.quality
        print "Got request for block {0}, running...".format(bounds),
        mandel_set = map(list, mandel.build_set(bounds, scale, quality))
        resp = interface.Message(interface.MSGT_RESPONSE, set=mandel_set, block=bounds)
        resp.send(s)
        print "done!"
