# WiiRazor
Successfully Brick your Nintendo Wii

# What is WiiRazor?
WiiRazor is my method of creating a bricker program/trojan for the Wii. Upon startup of the program, it will search for important system files in the console's NAND, overwrite them, and disassociate said directories with the operating system. Further powering of the device should render the Wii inoperable, as the files are corrupted and cannot be accessed anymore.

The source code has been compiled using DevKitPro.
Please read the official documentation before compiling this program

I had written this software because malware is what brought me into the cybersecurity field and I felt that with the scare-tactic that was Project Dandelion in 2019, it was about time developers could be warned of potential infections of homebrewed applications. That is why I included a splash screen for this, so it could be identified the moment an infected app was ran and the user could take measures to leave. If such is not included in your build of the program, it can be assumed that it has been tampered with.

This program will only work on homebrewed Wii devices, like any other custom application would. Since the NAND is restricted on devices without custom firmware, a cIOS with NAND access is required for the program to work. Since the steps to accomplish such are included in almost any modding guide, this implies that the program should work on any hacked device, given that everything has been initalized successfully.

# Disclaimer
THIS IS FOR EDUCATIONAL PURPOSES ONLY! I DO NOT TAKE RESPONSIBILITY FOR ANY UNINTENTIONAL HARM
DONE TO ANY CONSOLES! YOU RUN THE RESPONSIBILITY OF BRICKING YOUR DEVICE BY INSTALLING WIIRAZOR

# How to Install
## How to Install
- Download the `.Dol` file, `.xml` file, and image file.
- Place the files in `/apps/WiiRazor` on your SD card. If the directory doesn't exist, create it and place the files there.
- Boot up the console. Go to the Homebrew Channel on your Wii or load up the official forwarder channel on the Wii Menu.
- If booting from the Homebrew Channel, select **WiiRazor** from the list of applications.
- Wait 15-20 seconds, and the destruction process will begin.

