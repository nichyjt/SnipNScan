# SnipNScan  

## About  

Scan QR codes and read english text from a snapshot.  

A simple, lightweight desktop GUI application to make life easier for the many webinars where QR codes dont come with a link or when friends send me code via image instead of text when asking me to debug for them.  

## Features  

- QR Code Scanner

- Text Scanner

- URL Opening

## Quickstart

**Works ONLY for Linux**. Binary has been built and tested on a Ubuntu (POP OS) 21.04 machine.
Simply download the binary on releases page and launch via command line, `./SnipNScan`.

## Build From Source

Currently there are no plans to use Cmake or build makefiles to automate building from source, unless there is a demand. General guidelines for Linux-based installation below:

### Dependencies  

1. [OpenCV4](https://docs.opencv.org/4.x/d2/de6/tutorial_py_setup_in_ubuntu.html)  
Alternatively, `apt-get install libopencv-dev` if you don't want to compile from source

2. [wxWidgets](https://docs.wxwidgets.org/trunk/overview_install.html)  
Alternatively, `apt-get install libwxgtk3.0-gtk3-dev`

3. Tesseract  
`apt-get install libtesseract-dev`  

4. ZBar  
`apt-get install libzbar-dev`

### Windows

May release a Windows version of this application in the far future if there is demand. After wrangling and failing to cross compile for 3 days straight with wxWidgets & OpenCV on Linux, I have paused any plans thus far to compile to Windows. Anyone is welcome to build a toolchain file to build this app for Windows.

# License

This software is released under the GNU Affero GPL v3.
