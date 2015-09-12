#include <pspsdk.h>
#include <pspkernel.h>
#include <stdio.h> // sprintf()
#include <pspctrl.h> // sceCtrl*()

PSP_MODULE_INFO("idpsdump", 0, 0, 3);
PSP_MAIN_THREAD_ATTR(0);
PSP_HEAP_SIZE_KB(1024);

#include "../regedit_prx/regedit.h"
#define printf pspDebugScreenPrintf

#define key_number 0x0100 // 100/120, 101/121
#define key_offset 0x0038 // 100@38, 100@f0, 100@1a8, 101@60, 101@118

SceCtrlData pad;

void ExitCross(char*text)
{
	printf("%s, press X to exit...\n", text);
	do
	{
		sceCtrlReadBufferPositive(&pad, 1);
		sceKernelDelayThread(0.05*1000*1000);
	}
	while(!(pad.Buttons & PSP_CTRL_CROSS));
	sceKernelExitGame();
}

void ExitError(char*text, int delay, int error)
{
	printf(text, error);
	sceKernelDelayThread(delay*1000*1000);
	sceKernelExitGame();
}

int WriteFile(char*file, void*buf, int size)
{
	sceIoRemove(file);
	SceUID fd = sceIoOpen(file, PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);
	if (fd < 0)
		return fd;

	int written = sceIoWrite(fd, buf, size);
	sceIoClose(fd);

	return written;
}

int main(int argc, char*argv[])
{
	int i = 0;
	int paranoid = 0;
	char key_buffer[512];
	char idps_buffer[16];
	unsigned char idps_text_char_temp[1];
	unsigned char idps_text_char_1st[1];
	unsigned char idps_text_char_2nd[1];
	char idps_text_buffer[32] = "";

	sceKernelDelayThread(10000);
	sceCtrlReadBufferPositive(&pad, 1);
	sceKernelDelayThread(10000);
	if (pad.Buttons & PSP_CTRL_LTRIGGER)
		paranoid = 1;

	pspDebugScreenInit();
	pspDebugScreenClear(); // особо не нужно
	printf("PSP IDPS Dumper v0.3f by Yoti\n\n");

	SceUID mod = pspSdkLoadStartModule("regedit.prx", PSP_MEMORY_PARTITION_KERNEL);
	if (mod < 0)
		ExitError("Error: LoadStart() returned 0x%08x\n", 3, mod);

	ReadKey(key_number, key_buffer);

	printf(" Your IDPS is: ");
	if (paranoid == 1)
	{
		for (i=key_offset; i<key_offset+0x08; i++)
			printf("%02X", (u8)key_buffer[i]);
		for (i=key_offset; i<key_offset+0x08; i++)
			printf("XX");
		printf("\n");		
	}
	else
	{
		for (i=key_offset; i<key_offset+0x10; i++)
			printf("%02X", (u8)key_buffer[i]);
		printf("\n");
	}

	// binary
	for (i=key_offset; i<key_offset+0x10; i++)
		idps_buffer[i-key_offset]=key_buffer[i];
	WriteFile("ms0:/idps.bin", idps_buffer, 16);

	// text
	for (i=key_offset; i<key_offset+0x10; i++)
	{
		idps_text_char_temp[1]=key_buffer[i];
		idps_text_char_1st[1]=(idps_text_char_temp[1] & 0xf0) >> 4;
		idps_text_char_2nd[1]=(idps_text_char_temp[1] & 0x0f);

		// 1st half of byte
		if (idps_text_char_1st[1] < 0xA) // digit
			sprintf(idps_text_buffer, "%s%c", idps_text_buffer, idps_text_char_1st[1]+0x30);
		else // char
			sprintf(idps_text_buffer, "%s%c", idps_text_buffer, idps_text_char_1st[1]+0x37);

		// 2nd half of byte
		if (idps_text_char_2nd[1] < 0xA) // digit
			sprintf(idps_text_buffer, "%s%c", idps_text_buffer, idps_text_char_2nd[1]+0x30);
		else // char
			sprintf(idps_text_buffer, "%s%c", idps_text_buffer, idps_text_char_2nd[1]+0x37);
	}
	WriteFile("ms0:/idps.txt", idps_text_buffer, 32);

	ExitCross("\nDone");
	return 0;
}