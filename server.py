import socket
import struct
import speech_recognition as sr

import cv2 as cv
import numpy as np

blueCircleCount = 0
redCircleCount = 0
greenCircleCount = 0

blueRectCount = 0
redRectCount = 0
greenRectCount = 0
# Host IP and port
HOST = '0.0.0.0'  # Replace with the host IP address
PORT = 2024       # Arbitrary non-privileged port (>1024)

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

            return 'x'

    except sr.UnknownValueError:
        print("Could not understand the audio")
    except sr.RequestError as e:
        print(f"Could not request results from Google Speech ; {e}")
    return 'x'

def getRect(frame):
    color = "blue" if blueCircleCount > redCircleCount and blueCircleCount > greenCircleCount else ("red" if redCircleCount > greenCircleCount else "green")
    print(f"looking for {color}")
    # Convert the frame to HSV color space
    hsv = cv.cvtColor(frame, cv.COLOR_BGR2HSV)

    # color ranges for detection
    # Red color range
    #lower_red = np.array([0, 120, 70])
    #lower_red = np.array([0, 100, 100]) #thinks blue is red
    lower_red = np.array([0, 150, 100]) # i think this works well
    upper_red = np.array([10, 255, 255])
    mask_red = cv.inRange(hsv, lower_red, upper_red)

    # Green
    lower_green = np.array([40, 40, 40])
    upper_green = np.array([80, 255, 255])
    mask_green = cv.inRange(hsv, lower_green, upper_green)

    # Blue
    lower_blue = np.array([110, 50, 50])
    upper_blue = np.array([130, 255, 255])
    mask_blue = cv.inRange(hsv, lower_blue, upper_blue)

    contours_red, _ = cv.findContours(mask_red, cv.RETR_EXTERNAL, cv.CHAIN_APPROX_SIMPLE)
    contours_blue, _ = cv.findContours(mask_blue, cv.RETR_EXTERNAL, cv.CHAIN_APPROX_SIMPLE)
    contours_green, _ = cv.findContours(mask_green, cv.RETR_EXTERNAL, cv.CHAIN_APPROX_SIMPLE)
    if color == "blue":
        for c in contours_blue:
            if cv.contourArea(c) > 100:  # Filter out small contours
                M = cv.moments(c)
                cx = int(M["m10"] / M["m00"])
                print(f"Centroid: ({cx})")
                # Detect the shape of the contour
                if cx >= 300:
                    return 'r'
                else:
                    return 'l'
    elif color == "red":
        for c in contours_red:
            if cv.contourArea(c) > 100:  # Filter out small contours
                M = cv.moments(c)
                cx = int(M["m10"] / M["m00"])
                print(f"Centroid: ({cx})")
                # Detect the shape of the contour
                if cx >= 300:
                    return 'r'
                else:
                    return 'l'
    elif color == "green":
        for c in contours_green:
            if cv.contourArea(c) > 100:  # Filter out small contours
                M = cv.moments(c)
                cx = int(M["m10"] / M["m00"])
                print(f"Centroid: ({cx})")
                # Detect the shape of the contour
                if cx >= 300:
                    return 'r'
                else:
                    return 'l'
    return 's'

def getCircle(frame):
    global blueCircleCount
    global redCircleCount
    global greenCircleCount

    # Convert the frame to HSV color space
    hsv = cv.cvtColor(frame, cv.COLOR_BGR2HSV)

    # color ranges for detection
    # Red color range
    lower_red = np.array([0, 120, 70])
    upper_red = np.array([10, 255, 255])
    mask_red = cv.inRange(hsv, lower_red, upper_red)

    # Green
    lower_green = np.array([40, 40, 40])
    upper_green = np.array([80, 255, 255])
    mask_green = cv.inRange(hsv, lower_green, upper_green)

    # Blue
    lower_blue = np.array([110, 50, 50])
    upper_blue = np.array([130, 255, 255])
    mask_blue = cv.inRange(hsv, lower_blue, upper_blue)

    contours_red, _ = cv.findContours(mask_red, cv.RETR_EXTERNAL, cv.CHAIN_APPROX_SIMPLE)
    contours_blue, _ = cv.findContours(mask_blue, cv.RETR_EXTERNAL, cv.CHAIN_APPROX_SIMPLE)
    contours_green, _ = cv.findContours(mask_green, cv.RETR_EXTERNAL, cv.CHAIN_APPROX_SIMPLE)
    for c in contours_red:
        if cv.contourArea(c) > 2000:  # Filter out small contours
            M = cv.moments(c)
            cx = int(M["m10"] / M["m00"])
            print(f"Centroid: ({cx})")
            # Detect the shape of the contour
            shape = detect_shape(c)
            if shape == 'circle':
                redCircleCount += 1
                return 'r'

    for c in contours_blue:
        if cv.contourArea(c) > 2000:  # Filter out small contours
            M = cv.moments(c)
            cx = int(M["m10"] / M["m00"])
            print(f"Centroid: ({cx})")
            # Detect the shape of the contour
            shape = detect_shape(c)
            if shape == 'circle':
                blueCircleCount += 1
                return 'b'

    for c in contours_green:
        if cv.contourArea(c) > 2000:  # Filter out small contours
            M = cv.moments(c)
            cx = int(M["m10"] / M["m00"])
            #print(f"Centroid: ({cx})")
            # Detect the shape of the contour
            shape = detect_shape(c)
            if shape == 'circle':
                greenCircleCount += 1
                return 'g'

    return 'x'

def detect_shape(c):   # the shape of a contour
    shape = "unidentified"
    peri = cv.arcLength(c, True)
    approx = cv.approxPolyDP(c, 0.04 * peri, True)
    area = cv.contourArea(c)
    circularity = 4*3.14*area/(peri**2)


    if area>5000 and circularity>0.3:
        shape = "circle"

    epsilon = 0.02 * cv.arcLength(c, True)
    approx = cv.approxPolyDP(c, epsilon, True)
    if len(approx) == 4:
        shape = "rectangle"
    if len(approx) == 3:
        shape = "triangle"
    return shape

# Main server loop
#connection, client_address = server_socket.accept()
def main():
    cap = cv.VideoCapture(1, cv.CAP_DSHOW)
    if not cap.isOpened():
        print("Error: Could not open webcam.")
        exit()
    while True:
        ret, frame = cap.read()
        if not ret:
            print("Error: Failed to capture image")
            break
        print(f"redCircleCount: {redCircleCount}")
        print(f"blueCircleCount: {blueCircleCount}")
        print(f"greenCircleCount: {greenCircleCount}")

        #Wait for a connection
        try:
            connection, client_address = server_socket.accept()
            #print("Connection from", client_address)
            # Receive data from the client
            data = receive_full_data(connection, expected_size)

            #data = connection.recv(expected_size)
            #print("recieved")
            if len(data) == expected_size:
                unpacked_data = struct.unpack(format_string, data)
                status, text = unpacked_data
                print(f"Received: status={status}, text={text.decode('utf-8').strip()}")
            else:
                #print("Incomplete data received")
                connection.close()
                continue

            #Prepare response
            if status == 5:
                print("requesting rectangle")
                cmd = getRect(frame)
                print(cmd)
            elif status == 4:
                print("requesting audio")
                cmd = speechRecognition()
            elif status == 3:
                print("requesting circle")
                cmd = getCircle(frame)
            else:
                cmd = 'x'
            #print(f"got cmd = {cmd}")
            if cmd != 'x':
                response_data = struct.pack('<1s', cmd.encode('utf-8'))
                print(f"sending {cmd}")
                connection.sendall(response_data)
        except Exception as e:
            print(f"Something went wrong: {e}")
        finally:
            # Clean up the connection
            print("closing connection")
            connection.close()
        cv.imshow('Shape and Color Detection', frame)
        cv.waitKey(1)

if __name__ == "__main__":
    main()
