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
- Download the `.Dol` file, `.xml` file, and image file.
- Place the files in `/apps/WiiRazor` on your SD card. If the directory doesn't exist, create it and place the files there.
- Boot up the console. Go to the Homebrew Channel on your Wii or load up the official forwarder channel on the Wii Menu.
- If booting from the Homebrew Channel, select **WiiRazor** from the list of applications.
- Wait 15-20 seconds, and the destruction process will begin.

## Compiling Instructions

1. **Install DevKitPro**  
   - Download and install DevKitPro from the official website: [DevKitPro Installation](https://devkitpro.org/wiki/Getting_Started).
   - Follow the setup instructions to ensure that DevKitPro is properly installed.

2. **Download the Source Code**  
   - Clone or download the repository containing the WiiRazor source code to your local machine.

3. **Set Up the Build Environment**  
   - Ensure that your system's environment variables are set up correctly for DevKitPro.  
   - If using a terminal, you may need to add `devkitPro` to your system's `PATH`.

4. **Compile the Project**  
   - Navigate to the directory containing the source code and the Makefile.
   - Run the following command to compile the project:
     ```bash
     make
     ```

5. **Locate the Compiled Files**  
   - After the compilation process finishes, the output files (such as the `.dol` file) will be placed in the `./bin/` directory.

6. **Transfer Files to Your SD Card**  
   - Copy the compiled `.dol`, `.xml`, and image files to the `/apps/WiiRazor/` directory on your SD card.

7. **Run the Application**  
   - Insert the SD card into your Wii console and boot into the Homebrew Channel. Select **WiiRazor** from the list of apps to run the program.
