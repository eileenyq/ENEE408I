import socket
import struct
import speech_recognition as sr

# Host IP and port
HOST = '0.0.0.0'  # Replace with the host IP address
PORT = 2022       # Arbitrary non-privileged port (>1024)

# Create a TCP/IP socket
server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

# Bind the socket to the port
server_address = (HOST, PORT)
server_socket.bind(server_address)

# Listen for incoming connections
server_socket.listen(5)
print(f"Server started. Listening on {HOST}:{PORT}")

# Define the format string for the struct
format_string = '<h50s'

expected_size = struct.calcsize(format_string)
audio_cmd = 'x'
# Function to receive the exact number of bytes expected
def receive_full_data(connection, size):
    data = b''
    while len(data) < size:
        packet = connection.recv(size - len(data))
        if not packet:
            break
        data += packet
    return data

def speechRecognition():
    recognizer = sr.Recognizer()

    with sr.Microphone() as source:
        print("Listening for direction...")
        recognizer.adjust_for_ambient_noise(source, duration=1)
        audio = recognizer.listen(source)

    try:
        command = recognizer.recognize_google(audio)
        print("You said:", command)
        if "left" in command:
            print("Taking the left path.")
            
            # Add code to move robot left
            return 'l'
            
        elif "right" in command:
            print("Taking the right path.")
        
            # Add code to move robot right
            return 'r'
        else:
            print("Unrecognized word, defaulting to locate source.")

    except sr.UnknownValueError:
        print("Could not understand the audio")
    except sr.RequestError as e:
        print(f"Could not request results from Google Speech ; {e}")
    return 'x'

def getImage():
    return 'd'

# Main server loop
#connection, client_address = server_socket.accept()
def main(): 
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
            
            #Prepare response
            if status == 5:
                print("requesting image")
                cmd = getImage()
            elif status == 4:
                print("requesting audio")
                cmd = speechRecognition()
            else:
                cmd = 'x'
            response_data = struct.pack('<1s', cmd.encode('utf-8'))
            connection.sendall(response_data)
        except Exception as e:
            print(f"Something went wrong: {e}")
        finally:
            # Clean up the connection
            connection.close()

if __name__ == "__main__": 
    main()
