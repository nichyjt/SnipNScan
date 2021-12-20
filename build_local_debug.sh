#!/bin/bash
# This script is meant to simplify debugging and 'automate' compilation locally on a Linux machine.
if g++ ./src/*.cpp ./src/*.h `wx-config --cxxflags --libs std` `pkg-config --cflags --libs opencv4 tesseract zbar` -o SnipNScan; then
    ./SnipNScan; 
    du -h SnipNScan; # Show hard disk usage for analysis 
fi