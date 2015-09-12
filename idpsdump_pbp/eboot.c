#include <pspsdk.h>
#include <pspkernel.h>
#include <stdio.h> // sprintf()

PSP_MODULE_INFO("idpsdump", 0, 0, 2);
PSP_MAIN_THREAD_ATTR(0);
PSP_HEAP_SIZE_KB(20480);

#include "../regedit_prx/regedit.h"
#define printf pspDebugScreenPrintf

#include <pspctrl.h>
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

	sceKernelDelayThread(10000);
	sceCtrlReadBufferPositive(&pad, 1);
	sceKernelDelayThread(10000);
	if (pad.Buttons & PSP_CTRL_LTRIGGER)
		paranoid = 1;

	pspDebugScreenInit();
	pspDebugScreenClear(); // особо не нужно
	printf("Welcome to IdpsDump!\n\n");

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

	for (i=0x60; i<0x60+0x10; i++)
		idps101[i-0x60]=key101[i];
	WriteFile("ms0:/idps.bin", idps101, 16);

	ExitCross("\nDone");
	return 0;
}