#include <pspsdk.h>
#include <pspkernel.h>
#include <stdio.h> // sprintf()
#include <pspctrl.h> // sceCtrl*()

#define VER_MAJOR 0
#define VER_MINOR 5

PSP_MODULE_INFO("idpsdump", 0, VER_MAJOR, VER_MINOR);
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
	printf("PSP IDPS Dumper v%i.%i by Yoti\n\n", VER_MAJOR, VER_MINOR);

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
	}
	else
	{
		for (i=key_offset; i<key_offset+0x10; i++)
			printf("%02X", (u8)key_buffer[i]);
	}
	printf("\n\n");

	printf(" It seems that you are using ");
	if (key_buffer[key_offset+0x04] == 0x00)
		printf("PlayStation Portable");
	else if (key_buffer[key_offset+0x04] == 0x01)
		printf("PlayStation Vita"); // old, 3g, slim...
	else
		printf("UNKNOWN_%02X model", key_buffer[key_offset+0x04]); // нет инфы по VTE-XXXX/DOL-1001
	printf("\n");

	printf(" Your motherboard is ");
	switch(key_buffer[key_offset+0x07])
	{
		case 0x01:
			printf("TA-079/081 (PSP-1000)"); // также DOL-1001???
			break;
		case 0x02:
			printf("TA-082/086 (PSP-1000)");
			break;
		case 0x03:
			printf("TA-085/088 (PSP-2000)");
			break;
		case 0x04:
			printf("TA-090v2/092 (PSP-3000)");
			break;
		case 0x05:
			printf("TA-091 (PSP-N1000)");
			break;
		case 0x06:
			printf("TA-093 (PSP-3000)");
			break;
		//case 0x07:
		//	printf("???");
		//	break;
		case 0x08:
			printf("TA-095/095v2 (PSP-3000)");
			break;
		case 0x09:
			printf("TA-096/097 (PSP-E1000)");
			break;
		case 0x10:
			printf("IRS-002 (PCH-1000)");
			break;
		case 0x14:
			printf("USS-1001 (PCH-2000)");
			break;
		default:
			printf("UNKNOWN_%02X", key_buffer[key_offset+0x07]); // VTE-XXXX
			break;
	}
	printf("\n");

	printf(" And your region is ");
	switch(key_buffer[key_offset+0x05])
	{
		case 0x03:
			printf("Japan");
			break;
		case 0x04:
			printf("North America");
			break;
		case 0x05:
			printf("Europe/East/Africa");
			break;
		case 0x06:
			printf("Korea");
			break;
		case 0x07: // PCH-xx03 VTE-1016
			printf("Great Britain/United Kingdom");
			break;
		case 0x08:
			printf("Mexica/Latin America");
			break;
		case 0x09:
			printf("Australia/New Zeland");
			break;
		case 0x0A:
			printf("Hong Kong/Singapore");
			break;
		case 0x0B:
			printf("Taiwan");
			break;
		case 0x0C:
			printf("Russia");
			break;
		case 0x0D:
			printf("China");
			break;
		default:
			printf("UNKNOWN_%02X", key_buffer[key_offset+0x05]);
			break;
	}
	printf("\n\n");

	printf(" IF YOU SEE ANY UNKNOWN VALUES PLZ CONTACT WITH ME\n");

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
			sprintf(idps_text_buffer, "%s%02X", idps_text_buffer, idps_text_char_1st[1]+0x30);
		else // char
			sprintf(idps_text_buffer, "%s%02X", idps_text_buffer, idps_text_char_1st[1]+0x37);

		// 2nd half of byte
		if (idps_text_char_2nd[1] < 0xA) // digit
			sprintf(idps_text_buffer, "%s%02X", idps_text_buffer, idps_text_char_2nd[1]+0x30);
		else // char
			sprintf(idps_text_buffer, "%s%02X", idps_text_buffer, idps_text_char_2nd[1]+0x37);
	}
	WriteFile("ms0:/idps.txt", idps_text_buffer, 32);

	ExitCross("\nDone");
	return 0;
}