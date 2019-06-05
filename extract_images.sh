#!/bin/bash

FLIR_JPEG="$1"
base_name="$(basename -- $FLIR_JPEG .jpg)"

exiftool "$FLIR_JPEG" -b -EmbeddedImage > "${base_name}_visible.jpg"
exiftool "$FLIR_JPEG" -b -RawThermalImage | convert - -interlace none -depth 16 r:- | ./flir_tool > "${base_name}_radiometric.pgm"

