import cv2 as cv
import numpy as np

cap = cv.VideoCapture(0)

if not cap.isOpened():
    print("Error: Could not open webcam.")
    exit()

def detect_shape(c):   # the shape of a contour
    shape = "unidentified"
    peri = cv.arcLength(c, True)
    approx = cv.approxPolyDP(c, 0.04 * peri, True)
  
    if len(approx) == 3:      # Triangle
        shape = "triangle"
        
    elif len(approx) == 4:      # rectangle
        (x, y, w, h) = cv.boundingRect(approx)
        ar = w / float(h)
        shape = "square" if 0.95 <= ar <= 1.05 else "rectangle"
        
    elif len(approx) > 4:
        shape = "circle"
    
    return shape

while True:
    
    ret, frame = cap.read()
    if not ret:
        print("Error: Failed to capture image")
        break

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
    lower_blue = np.array([100, 150, 0])
    upper_blue = np.array([140, 255, 255])
    mask_blue = cv.inRange(hsv, lower_blue, upper_blue)

    # Combine color
    mask_combined = cv.bitwise_or(mask_red, mask_green)
    mask_combined = cv.bitwise_or(mask_combined, mask_blue)

    redCircleCount = 0;
    blueCircleCount = 0;
    greenCircleCount = 0;

    # contours
    contours, _ = cv.findContours(mask_blue, cv.RETR_EXTERNAL, cv.CHAIN_APPROX_SIMPLE)

    for c in contours:
        if cv.contourArea(c) > 500:  # Filter out small contours
            # Detect the shape of the contour
            shape = detect_shape(c)
            if shape == 'circle':
                blueCircleCount += 1
            x, y, w, h = cv.boundingRect(c)
            cv.drawContours(frame, [c], -1, (255, 255, 255), 2)
            cv.putText(frame, f"{blueCircleCount} blue " + shape, (x, y - 10), cv.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 2)

    contours, _ = cv.findContours(mask_green, cv.RETR_EXTERNAL, cv.CHAIN_APPROX_SIMPLE)

    for c in contours:
        if cv.contourArea(c) > 500:  # Filter out small contours
            # Detect the shape of the contour
            shape = detect_shape(c)
            if shape == 'circle':
                greenCircleCount += 1
            x, y, w, h = cv.boundingRect(c)
            cv.drawContours(frame, [c], -1, (255, 255, 255), 2)
            cv.putText(frame, f"{greenCircleCount} green " + shape, (x, y - 10), cv.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 2)

    contours, _ = cv.findContours(mask_red, cv.RETR_EXTERNAL, cv.CHAIN_APPROX_SIMPLE)

    for c in contours:
        if cv.contourArea(c) > 500:  # Filter out small contours
            # Detect the shape of the contour
            shape = detect_shape(c)
            if shape == 'circle':
                redCircleCount += 1
            x, y, w, h = cv.boundingRect(c)
            cv.drawContours(frame, [c], -1, (255, 255, 255), 2)
            cv.putText(frame,  f"{redCircleCount} red " + shape, (x, y - 10), cv.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 2)
    
    # Display the resulting frame
    cv.imshow('Shape and Color Detection with OCR', frame)
            
            

    # Display the resulting frame
    cv.imshow('Shape and Color Detection', frame)

    print(redCircleCount)
    print(greenCircleCount)
    print(blueCircleCount)
    # Exit with E 
    if cv.waitKey(1) & 0xFF == ord('E'):
        break

# release the camera 
cap.release()
cv.destroyAllWindows()