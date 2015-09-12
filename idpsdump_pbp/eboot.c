#include <pspsdk.h>
#include <pspkernel.h>
#include <stdio.h> // sprintf()
#include <pspctrl.h> // sceCtrl*()

PSP_MODULE_INFO("idpsdump", 0, 0, 3);
PSP_MAIN_THREAD_ATTR(0);
PSP_HEAP_SIZE_KB(1024);

#include "../regedit_prx/regedit.h"
#define printf pspDebugScreenPrintf

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

int pspSdkLoadStartModule_Smart(const char*file)
{
	SceUID module_file;
	u8 module_type = 0;

	module_file = sceIoOpen(file, PSP_O_RDONLY, 0777);
	if (module_file >= 0)
	{
		sceIoLseek(module_file, 0x7C, PSP_SEEK_SET);
		sceIoRead(module_file, &module_type, 1);
		sceIoClose(module_file);

		if (module_type == 0x02)
			return pspSdkLoadStartModule(file, PSP_MEMORY_PARTITION_KERNEL);
		else if (module_type == 0x04)
			return pspSdkLoadStartModule(file, PSP_MEMORY_PARTITION_USER);
		else
			return -2; // неизвестный тип
	}
	else
		sceIoClose(module_file);

	return -1; // нет файла
}

int main(int argc, char*argv[])
{
	int i = 0;
	int paranoid = 0;
	char key101[256];
	char idps101[16];
	unsigned char char101[1];
	unsigned char char101_1[1];
	unsigned char char101_2[1];
	/*unsigned*/ char text101[32] = "";

	sceKernelDelayThread(10000);
	sceCtrlReadBufferPositive(&pad, 1);
	sceKernelDelayThread(10000);
	if (pad.Buttons & PSP_CTRL_LTRIGGER)
		paranoid = 1;

	pspDebugScreenInit();
	pspDebugScreenClear(); // особо не нужно
	printf("Welcome to IDPS Dumper v0.3 by Yoti\n\n");

	SceUID mod = pspSdkLoadStartModule_Smart("regedit.prx");
	if (mod < 0)
		ExitError("Error: LoadStart() returned 0x%08x\n", 3, mod);

	ReadKey(0x101, key101);

	printf(" Your IDPS is: ");
	if (paranoid == 1)
	{
		printf("h1dd3n!\n");
	}
	else
	{
		for (i=0x60; i<0x60+0x10; i++)
			printf("%02X", (u8)key101[i]);
		printf("\n");
	}

	// binary
	for (i=0x60; i<0x60+0x10; i++)
		idps101[i-0x60]=key101[i];
	WriteFile("ms0:/idps.bin", idps101, 16);

	// text
	for (i=0x60; i<0x60+0x10; i++)
	{
		char101[1]=key101[i];
		char101_1[1]=(char101[1] & 0xf0) >> 4;
		char101_2[1]=(char101[1] & 0x0f);

		// 1st half of byte
		if (char101_1[1] < 0xA) // digit
			sprintf(text101, "%s%c", text101, char101_1[1]+0x30);
		else // char
			sprintf(text101, "%s%c", text101, char101_1[1]+0x37);

		// 2nd half of byte
		if (char101_2[1] < 0xA) // digit
			sprintf(text101, "%s%c", text101, char101_2[1]+0x30);
		else // char
			sprintf(text101, "%s%c", text101, char101_2[1]+0x37);
	}
	WriteFile("ms0:/idps.txt", text101, 32);

	ExitCross("\nDone");
	return 0;
}