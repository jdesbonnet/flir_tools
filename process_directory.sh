#!/bin/bash
shopt -s nullglob;
files=("$1"/*.jpg)
for f in "${files[@]}"; do 
	echo "f=$f"; 
	~/git/flir_tools/extract_images.sh "$f"
done
	

