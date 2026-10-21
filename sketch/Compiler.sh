#!/bin/bash

set -e

FQBN="esp32:esp32:esp32"
PORT="/dev/ttyUSB0"

echo "Please wait while the sketch is being compiled..."

if arduino-cli compile --fqbn "$FQBN" .; then
    echo "Sketch compiled successfully!"
else
    echo "Compilation failed. Firmware was not uploaded."
    exit 1
fi

echo
echo "Please wait while the firmware is being uploaded..."

if arduino-cli upload -p "$PORT" --fqbn "$FQBN" .; then
    echo "Firmware uploaded successfully!"
else
    echo "Firmware upload failed."
    exit 1
fi