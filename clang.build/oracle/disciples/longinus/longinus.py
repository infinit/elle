from __future__ import print_function

import sys
import itertools as it
#import pymongo

from twisted.internet.protocol import DatagramProtocol
from twisted.python import log

class NameServer(DatagramProtocol):

    clients = set()

    def handle_echo(self, (host, port), type, *args):
        if type == "sasp": #Same Address Same Port
            msg = "{}:{}".format(host, port)
            print("send {} to {}:{}".format(msg, host, port))
            self.transport.write("{}\n".format(msg), (host, port))
        elif type == "dadp": #Different Address Different Port
            pass
        elif type == "sadp": #Same Address Different Port
            self.transport.write(msg, (host, port))

    def handle_hello(self, (host, port), *args):
        me = "{}:{}".format(host, port)
        self.clients.add(me)
        self.transport.write("{}\n".format(me), (host, port))
        print("send {} to {}".format(me, (host, port)))
        # for (what, who) in it.permutations(self.clients, 2):
        #     host, port = who.split(":")
        #     port = int(port)
        #     print("{} -> {}".format(who, what))
        #     self.transport.write("{}\n".format(what), (host, port))

    def handle_quit(self, (host, port), *args):
        self.clients.remove(me)
        return

    def handle_ping(self, (host, port), *args):
        self.transport.write("pong\n", (host, port))

    def datagramReceived(self, data, (host, port)):
        cmd_line = data.split()
        cmd = cmd_line[0]
        print("{} from {}:{}".format(cmd, host, port))
        hdl = getattr(self, "handle_{}".format(cmd), None)
        if hdl is not None:
            hdl((host, port), *cmd_line[1:])

