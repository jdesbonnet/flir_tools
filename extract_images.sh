#!/bin/bash

FLIR_TOOL=flir_tool
FLIR_JPEG="$1"

s="$FLIR_JPEG"
base_name="${FLIR_JPEG%.*}"


exiftool "$FLIR_JPEG" -b -EmbeddedImage > "${base_name}_visible.jpg"
exiftool "$FLIR_JPEG" -b -RawThermalImage | convert - -interlace none -depth 16 r:- | ${FLIR_TOOL} > "${base_name}_radiometric.pgm"

