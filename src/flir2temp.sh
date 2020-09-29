#!/bin/bash

# Convert a Flir radiometic SEQ or JPEG file into a temperature image.
# Assuming 95% emissivity.
# Based on information gleaned from:
# From https://exiftool.org/forum/index.php/topic,4898.msg23944.html#msg23944


input_image=$1
base_name=${input_image%.*}
output_image="${base_name}.C.pgm"


echo "base_name=$base_name"
echo "output=$output_image"

# get FLIR metadata using exiftool
metadata=$(exiftool -Flir:all "$1")
type=$(echo "$metadata" | grep "Raw Thermal Image Type" | cut -d: -f2)
if [ "$type" != " TIFF" ]
    then
        echo "only for RawThermalImage=TIFF"
        exit 1
fi

# Extract constants of interest from image
R1=$(echo "$metadata" | grep "Planck R1" | cut -d: -f2)
R2=$(echo "$metadata" | grep "Planck R2" | cut -d: -f2)
B=$(echo "$metadata" | grep "Planck B" | cut -d: -f2)
O=$(echo "$metadata" | grep "Planck O" | cut -d: -f2)
F=$(echo "$metadata" | grep "Planck F" | cut -d: -f2)

# Remove leading space
R1=`echo $R1 | sed 's/ *$//g'`
R2=`echo $R2 | sed 's/ *$//g'`
B=`echo $B | sed 's/ *$//g'`
O=`echo $O | sed 's/ *$//g'`
F=`echo $F | sed 's/ *$//g'`


echo "R1=$R1 R2=$R2 B=$B O=$O F=$F"

CONSTANTS="-C R1=$R1 -C R2=$R2 -C B=$B -C O=$O -C F=$F"
# Use imagemagick convert to convert the TIFF file output from exiftool into
# PGM which is easy for the flir_tool.c to read.
exiftool "$1" -b -RawThermalImage | convert - pgm:- > t.pgm
exiftool "$1" -b -RawThermalImage | convert - pgm:- | ./flir_tool -u C -m 100 -f pgm_ascii $CONSTANTS  > $output_image

