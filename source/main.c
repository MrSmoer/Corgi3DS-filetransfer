#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <3ds.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
	gfxInitDefault();
	consoleInit(GFX_TOP, NULL);


	volatile char* magicreg = (char*)0xDEADBEEF;
	unsigned int packetsize = 0;
	char* data = (char*)-1;
	int state = 0;
	printf("Ready to transfer files!\n");
	int sp;

    // Inline assembly to get the value of the stack pointer
    __asm__ volatile ("mov %0, sp" : "=r" (sp));
	printf("Stack Pointer (sp) = 0x%x\n", sp);
	printf("Magicreg = %p\n", &magicreg);
	// Main loop
	while (aptMainLoop())
	{
		gspWaitForVBlank();
		gfxSwapBuffers();
		hidScanInput();
		// fwrite("abcdefghijk", 1, 3, file);
		// okay, so i have to get the size of the file i want to receive
		// i have magicreg for synchronization
		if(magicreg != (char*) 0xDEADBEEF && state == 0){
			packetsize = (unsigned int) magicreg;
			printf("Size is %d", packetsize);
			data = malloc(packetsize);
			
			magicreg = data;
			state++;
			printf("size exchanged\n");
			continue;
		}

		if(magicreg != data && state == 1){
			if(magicreg == (char *)0xBEEFDEAD){
				// First field is size of name including nullbyte
				unsigned char filenamesize = *(unsigned char*)data;
				printf("Filename is %d bytes long", filenamesize);
				char* filenamestr = malloc(filenamesize);

				for(int i = 0; i< filenamesize; i++){
					// offset 1 for filenamesize
					filenamestr[i] = data[1+i];
				}
				printf("Opening %s\n", filenamestr);
				FILE *file = fopen(filenamestr, "wb");
				printf("Writing to %s\n", filenamestr);

				fwrite(data+filenamesize+1, 1, packetsize-filenamesize-1, file);
				printf("Copied data");
				free(filenamestr);
				free(data);
				fclose(file);
				printf("Closed file\n");

				magicreg = (char*)0xDEADBEEF;
				printf("Ready for new transfer\n");
				state = 0;
			} else {
				printf("Transfer not finished properly\n");
				state++;
			}
			continue;
		}
		
		
		// Your code goes here
		u32 kDown = hidKeysDown();
		if (kDown & KEY_START)
			break; // break in order to return to hbmenu
	}
		

	gfxExit();
	return 0;
}
