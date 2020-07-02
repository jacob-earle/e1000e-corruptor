#!/bin/sh
# Renaming command line arguments for ease of use
PID=$1
VA=$2
OFFSET=$3

# Running read_mapping on the desired pid with the given physical address
sudo ./read_mapping $PID $VA | grep "PFN" | sed 's/PFN: /#define PAGE_TO_CORRUPT /; s/$/ULL/' > addresses.h
echo "#define OFFSET 0x""$OFFSET""ULL" >> addresses.h
