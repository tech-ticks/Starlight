import socket
import sys
import os
import time

HOST = sys.argv[1] if len(sys.argv) > 1 else os.environ["IP"]
PORT = int(sys.argv[2]) if len(sys.argv) > 2 else 8989

print(f"Connecting to {HOST}:{PORT}...", end='', flush="True")
connected = False
while not connected:
    try:
        print(".", end='', flush="True")
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        server_address = (HOST, PORT)
        sock.connect(server_address)
        connected = True
    except:
        time.sleep(1)

print(" Connected!")

while True:
    data = sock.recv(32)
    print(data.decode("utf-8"), end='', flush="True")
