import socket
import struct

def sendReq(s, num):
    string = "test string"
    req_data = struct.pack('<h50s', num, string.encode('utf-8'))
    s.send(req_data)

def main():
    while True:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect(("127.0.0.1", 2024))

        sendReq(s, 5)
        data = s.recv(1024)
        print(data.decode('utf-8'))
        s.close()

if __name__ == "__main__":
    main()
