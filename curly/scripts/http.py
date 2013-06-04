import sys
import SimpleHTTPServer

def test(HandlerClass = SimpleHTTPServer.SimpleHTTPRequestHandler,
         ServerClass = SimpleHTTPServer.BaseHTTPServer.HTTPServer):
    SimpleHTTPServer.BaseHTTPServer.test(HandlerClass, ServerClass)

try:
    test()
except KeyboardInterrupt:
    sys.exit(0)
