import socket

def start_client():
    host = '172.29.130.204'  # Server address
    port = 2020  # Server port

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as client_socket:
        client_socket.connect((host, port))

        message = "Hello from client!"
        client_socket.send(message.encode('utf-8'))

        data = client_socket.recv(1024)
        response = data.decode('utf-8')
        print(f"Received from server: {response}")

if __name__ == "__main__":
    start_client()