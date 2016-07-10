from time import time
import socketserver

class MyTCPHandler(socketserver.BaseRequestHandler):
    """
    The RequestHandler class for our server.

    It is instantiated once per connection to the server, and must
    override the handle() method to implement communication to the
    client.
    """

    def handle(self):
        print("received connection")
        # self.request is the TCP socket connected to the client
        sample_count = 0
        start = time()
        while True:
            self.data = self.request.recv(1024).strip()
            sample_count += 1
            if sample_count % 100 == 0:
                span = time() - start
                start = time()
                print("sampling rate: {:.2f} hz".format(100.0 / span))

if __name__ == "__main__":
    HOST, PORT = "0.0.0.0", 9999

    # Create the server, binding to localhost on port 8080
    server = socketserver.TCPServer((HOST, PORT), MyTCPHandler)

    # Activate the server; this will keep running until you
    # interrupt the program with Ctrl-C
    server.serve_forever()

