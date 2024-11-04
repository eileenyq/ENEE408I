import socket
import struct
import sys
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

# Main server loop
while True:
    # Wait for a connection
    connection, client_address = server_socket.accept()

    try:
        var = input("Please enter something: ")
        print(f"var is {var}")
        response_data = struct.pack('<1s', var[0].encode('utf-8'))
        connection.sendall(response_data)
    except Exception as e:
        print(f"Something went wrong: {e}")
    finally:
        connection.close()
