from qreader import QReader
import cv2
import sys


# Create a QReader instance
qreader = QReader()

# Get the image that contains the QR code
image = cv2.imread(sys.argv[1])

# Use the detect_and_decode function to get the decoded QR data
decoded_text = qreader.detect_and_decode(image=image)

print(decoded_text)