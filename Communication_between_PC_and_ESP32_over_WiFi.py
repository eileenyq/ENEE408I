import socket
import struct

# Host IP and port
HOST = '172.20.10.3'  # Replace with the host IP address
PORT = 2024       # Arbitrary non-privileged port (>1024)

# Create a TCP/IP socket
server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Bind the socket to the port
server_address = (HOST, PORT)
server_socket.bind(server_address)

# Listen for incoming connections
server_socket.listen(5)
print(f"Server started. Listening on {HOST}:{PORT}")

# Define the format string for the struct
format_string = '<h50s'

expected_size = struct.calcsize(format_string)

# Function to receive the exact number of bytes expected
def receive_full_data(connection, size):
    data = b''
    while len(data) < size:
        packet = connection.recv(size - len(data))
        if not packet:
            break
        data += packet
    return data


def getImage():
    return 5


# Main server loop
#connection, client_address = server_socket.accept()
while True:
    # Wait for a connection
    try:
        #print("Connection from", client_address)
        connection, client_address = server_socket.accept()

        # Receive data from the client
        data = receive_full_data(connection, expected_size)
        if len(data) == expected_size:
            unpacked_data = struct.unpack(format_string, data)
            status, text = unpacked_data
            print(f"Received: status={status}, text={text.decode('utf-8').strip()}")
        else:
            print("Incomplete data received")
         # Prepare response
        num = "g"
        response_data = struct.pack('<1s', num.encode('utf-8'))
        connection.sendall(response_data)
    except Exception as e:
        print(f"Something went wrong: {e}")
    finally:
        # Clean up the connection
        connection.close()
