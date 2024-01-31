#include <pspsdk.h>
#include <pspkernel.h>
#include <pspidstorage.h>
#include <stdio.h> // sprintf()
#include <pspctrl.h> // sceCtrl*()
#include "libpspexploit.h"

#define VER_MAJOR 1
#define VER_MINOR 2
#define VER_BUILD ""

#define VAL_LENGTH 0x10
#define VAL_PUBLIC 0x0A
#define VAL_PRIVATE 0x06

PSP_MODULE_INFO("idpsdump", 0, VER_MAJOR, VER_MINOR);
PSP_MAIN_THREAD_ATTR(0);
PSP_HEAP_SIZE_KB(1024);

#define printf pspDebugScreenPrintf

#define key_number 0x0100 // 100/120, 101/121
#define key_offset 0x0038 // 100@38, 100@f0, 100@1a8, 101@60, 101@118

/*
	Model: Proto, SKU: DEM-3000, MoBo: IRT-001/IRT-002;
	Model: FatWF, SKU: PCH-1000, MoBo: IRS-002/IRS-1001;
	Model: Fat3G, SKU: PCH-1100, MoBo: IRS-002/IRS-1001;
	Model: Slim, SKU: PCH-2000, MoBo: USS-1001/USS-1002;
	Model: TV, SKU: VTE-1000, MoBo: DOL-1001/DOL-1002.

	No diff between FatWF and Fat3G.
	No diff between Vita TV (Asian) and PSTV (Western).
*/

SceCtrlData pad;

static KernelFunctions _ktbl; KernelFunctions* k_tbl = &_ktbl;
int (*_sceIdStorageReadLeaf)(int, void*) = NULL;
int (*_sceIdStorageCreateLeaf)(int) = NULL;
int (*_sceIdStorageWriteLeaf)(int, void*) = NULL;
int (*_sceIdStorageLookup)(int, int, void*, int) = NULL;
int (*_sceIdStorageFlush)(void) = NULL;
char idps_buffer[16];

void ExitCross(char*text) {
	printf("%s, press X to exit...\n", text);
	do {
		sceCtrlReadBufferPositive(&pad, 1);
		sceKernelDelayThread(0.05*1000*1000);
	}
	while(!(pad.Buttons & PSP_CTRL_CROSS));
	sceKernelExitGame();
}

void ExitError(char*text, int delay, int error) {
	printf(text, error);
	sceKernelDelayThread(delay*1000*1000);
	sceKernelExitGame();
}

int WriteFile(char*file, void*buf, int size) {
	sceIoRemove(file);
	SceUID fd = sceIoOpen(file, PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);
	if (fd < 0)
		return fd;

	int written = sceIoWrite(fd, buf, size);
	sceIoClose(fd);

	return written;
}

int prxIdStorageLookup(u16 key, u32 offset, void *buf, u32 len) {
	memset(buf, 0, len);
	int ret = _sceIdStorageLookup(key, offset, buf, len);
	return ret;
}

/*
 * DO NOT ENABLE it will fuck ya shit up!
 * int prxIdStorageWriteLeaf(u16 key, void *buf) {
	char tmp[512];
	int ret = _sceIdStorageReadLeaf(key, tmp);
	if (ret < 1)
		_sceIdStorageCreateLeaf(key);
	ret = _sceIdStorageWriteLeaf(key, buf);
	_sceIdStorageFlush();
	return ret;
}
*/

void elevatePrivs() {

	int k1 = pspSdkSetK1(0);
	int userLevel = pspXploitSetUserLevel(8);
	pspXploitRepairKernel();
	pspXploitScanKernelFunctions(k_tbl);


	_sceIdStorageReadLeaf = pspXploitFindFunction("sceIdStorage_Service", "sceIdStorage_driver", 0xEB00C509);
	_sceIdStorageCreateLeaf = pspXploitFindFunction("sceIdStorage_Service", "sceIdStorage_driver", 0x08A471A6);
	_sceIdStorageWriteLeaf = pspXploitFindFunction("sceIdStorage_Service", "sceIdStorage_driver", 0x1FA4D135);
	_sceIdStorageLookup = pspXploitFindFunction("sceIdStorage_Service", "sceIdStorage_driver", 0x6FE062D1);
	_sceIdStorageFlush = pspXploitFindFunction("sceIdStorage_Service", "sceIdStorage_driver", 0x3AD32523);

	
	//if(prxIdStorageWriteLeaf(key_number, idps_buffer)<0)
		//printf("WriteLeaf Failed!\n");

	if(prxIdStorageLookup(key_number, key_offset, idps_buffer, sizeof(idps_buffer))<0)
		printf("Lookup Failed!\n");

	pspSdkSetK1(k1);
	pspXploitSetUserLevel(userLevel);

}

int main(int argc, char*argv[]) {
	int i = 0;
	int paranoid = 0;
	unsigned char idps_text_char_tmp[1];
	unsigned char idps_text_char_1st[1];
	unsigned char idps_text_char_2nd[1];
	char idps_text_buffer[32] = "";

	sceKernelDelayThread(1000000);
	sceCtrlReadBufferPositive(&pad, 1);
	sceKernelDelayThread(1000000);
	if (pad.Buttons & PSP_CTRL_LTRIGGER)
		paranoid = 1;

	pspDebugScreenInit();
	pspDebugScreenClear(); // особо не нужно
	printf("PSP IDPS Dumper v%i.%i%s by Yoti\n\n", VER_MAJOR, VER_MINOR, VER_BUILD);

	if (VAL_PUBLIC + VAL_PRIVATE != VAL_LENGTH)
		ExitError("Length error 0x%02x", 5, VAL_PUBLIC + VAL_PRIVATE);

	int res = pspXploitInitKernelExploit();
	if(res == 0) 
		res = pspXploitDoKernelExploit();
		if(res == 0) 
			pspXploitExecuteKernel(elevatePrivs);
		else
			ExitError("Can't exploit function", 5, 1);

	printf(" Your IDPS is: ");
	for (i=0; i<VAL_PUBLIC; i++) {
		if (i == 0x04)
			pspDebugScreenSetTextColor(0xFF0000FF); // red #1
		else if (i == 0x05)
			pspDebugScreenSetTextColor(0xFFFF0000); // blue #2
		else if (i == 0x06)
			pspDebugScreenSetTextColor(0xFF0000FF); // red #3
		else if (i == 0x07)
			pspDebugScreenSetTextColor(0xFF00FF00); // green #4
		else if (i == 0x08)
			pspDebugScreenSetTextColor(0xFF007FFF); // orange #5
		else
			pspDebugScreenSetTextColor(0xFFFFFFFF); // white
		printf("%02X", (u8)idps_buffer[i]);
	}
	if (paranoid == 1) {
		for (i=0; i<VAL_PRIVATE; i++) {
			pspDebugScreenSetTextColor(0xFF777777); // gray
			printf("XX");
			pspDebugScreenSetTextColor(0xFFFFFFFF); // white
		}
	} else {
		for (i=0; i<VAL_PRIVATE; i++) {
			pspDebugScreenSetTextColor(0xFFFFFFFF); // white
			printf("%02X", (u8)idps_buffer[VAL_PUBLIC+i]);
		}
	}
	printf("\n\n");

	printf(" It seems that you are using ");
	pspDebugScreenSetTextColor(0xFF0000FF); // red
	if (idps_buffer[0x04] == 0x00)
		printf("PlayStation Portable");
	else if (idps_buffer[0x04] == 0x01) { // psv, vtv/pstv
		if (idps_buffer[0x06] == 0x00)
			printf("PlayStation Vita"); // fatWF/fat3G, slim
		else if (idps_buffer[0x06] == 0x02)
			printf("PlayStation/Vita TV"); // vtv, pstv
		else if (idps_buffer[0x06] == 0x06)
			printf("PlayStation/Vita TV"); // vtv, pstv (testkit)
		else
			printf("Unknown Vita 0x%02X", idps_buffer[0x06]);
	}
	else
		printf("Unknown PS 0x%02X", idps_buffer[0x04]);
	pspDebugScreenSetTextColor(0xFFFFFFFF); // white
	printf("\n");

	printf(" Your motherboard is ");
	pspDebugScreenSetTextColor(0xFF00FF00); // green
	if (idps_buffer[0x06] == 0x00) { // portable
		switch(idps_buffer[0x07]) {
			case 0x01:
				printf("TA-079/081 (PSP-1000)");
				break;
			case 0x02:
				printf("TA-082/086 (PSP-1000)");
				break;
			case 0x03:
				printf("TA-085/088 (PSP-2000)");
				break;
			case 0x04:
				printf("TA-090/092 (PSP-3000)");
				break;
			case 0x05:
				printf("TA-091 (PSP-N1000)");
				break;
			case 0x06:
				printf("TA-093 (PSP-3000)");
				break;
			case 0x07:
				printf("TA-094(?) (PSP-N1000)");
				break;
			case 0x08:
				printf("TA-095 (PSP-3000)");
				break;
			case 0x09:
				printf("TA-096/097 (PSP-E1000)");
				break;
			case 0x10:
				printf("IRS-002 (PCH-1000/1100)");
				break;
			case 0x11: // 3G?
			case 0x12: // WF?
				printf("IRS-1001 (PCH-1000/1100)");
				break;
			case 0x14:
				printf("USS-1001 (PCH-2000)");
				break;
			case 0x18:
				printf("USS-1002 (PCH-2000)");
				break;
			default:
				printf("Unknown MoBo 0x%02X", idps_buffer[0x07]);
				break;
		}
	} else if ((idps_buffer[0x06] == 0x02) || (idps_buffer[0x06] == 0x06)) { // home system
		switch(idps_buffer[0x07]) {
			case 0x01:
				printf("DOL-1001 (VTE-1000)");
				break;
			case 0x02:
				printf("DOL-1002 (VTE-1000)");
				break;
			default:
				printf("Unknown MoBo 0x%02X", idps_buffer[0x07]);
				break;
		}
	} else
		printf("Unknown type 0x%02X", idps_buffer[0x06]);
	pspDebugScreenSetTextColor(0xFF007FFF); // orange
	if ((u8)idps_buffer[0x08] == 0x8C)
		printf(" [QAF]");
	else
		printf(" [non-QAF]");
	pspDebugScreenSetTextColor(0xFFFFFFFF); // white
	//printf(" [Product Sub Code]");
	printf("\n");

	printf(" And your region is ");
	pspDebugScreenSetTextColor(0xFFFF0000); // blue
	switch(idps_buffer[0x05]) {
		case 0x00:
			printf("Proto");
			break;
		case 0x01:
			printf("DevKit");
			break;
		case 0x02:
			printf("TestKit");
			break;
		case 0x03:
			printf("Japan 00");
			break;
		case 0x04:
			printf("North America 01");
			break;
		case 0x05:
			printf("Europe/East/Africa 04");
			break;
		case 0x06:
			printf("Korea 05");
			break;
		case 0x07:
			printf("Great Britain/United Kingdom 03");
			break;
		case 0x08:
			printf("Mexica/Latin America 10");
			break;
		case 0x09:
			printf("Australia/New Zeland 02");
			break;
		case 0x0A:
			printf("Hong Kong/Singapore 06");
			break;
		case 0x0B:
			printf("Taiwan 07");
			break;
		case 0x0C:
			printf("Russia 08");
			break;
		case 0x0D:
			printf("China 09");
			break;
		default:
			printf("Unknown region 0x%02X", idps_buffer[0x05]);
			break;
	}
	pspDebugScreenSetTextColor(0xFFFFFFFF); // white
	//printf(" [Product Code]");
	printf("\n\n");

	// binary
	printf(" Saving as ms0:/idps.bin... ");
	if (WriteFile("ms0:/idps.bin", idps_buffer, 16) > 0)
		printf("OK");
	else
		printf("NG");
	printf("\n");

	// text
	for (i=0; i<0x10; i++) {
		idps_text_char_tmp[1]=idps_buffer[i];
		idps_text_char_1st[1]=(idps_text_char_tmp[1] & 0xf0) >> 4;
		idps_text_char_2nd[1]=(idps_text_char_tmp[1] & 0x0f);

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
	printf(" Saving as ms0:/idps.txt... ");
	if (WriteFile("ms0:/idps.txt", idps_text_buffer, 32) > 0)
		printf("OK");
	else
		printf("NG");
	printf("\n\n");

	printf(" https://github.com/yoti/psp_idpsdump/\n");

	ExitCross("\nDone");
	return 0;
}
