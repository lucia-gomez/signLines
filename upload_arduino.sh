#!/bin/bash

SKETCH_PATH="./paintPumps/paintPumps.ino"

# Specify the board and port
BOARD="arduino:samd:nano_33_iot"
PORT="/dev/ttyACM1"

# Check if the sketch file exists
if [ ! -f "$SKETCH_PATH" ]; then
    echo "Sketch file not found at $SKETCH_PATH"
    exit 1
fi

# Compile the sketch
echo "Compiling sketch..."
arduino-cli compile --fqbn $BOARD $SKETCH_PATH

# Check if compilation was successful
if [ $? -ne 0 ]; then
    echo "Compilation failed!"
    exit 1
fi

# Upload the sketch to the Arduino
echo "Uploading sketch to $PORT..."
arduino-cli upload -p $PORT --fqbn $BOARD $SKETCH_PATH

# Check if upload was successful
if [ $? -eq 0 ]; then
    echo "Upload successful!"
else
    echo "Upload failed!"
fi
