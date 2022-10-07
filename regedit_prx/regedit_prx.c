#include <pspsdk.h>
#include <pspkernel.h>
#include <pspidstorage.h>
#include <string.h> // memset()

PSP_MODULE_INFO("regedit_prx", 0x1006, 2, 0);
PSP_MAIN_THREAD_ATTR(0);

int module_start(SceSize args, void *argp) {
	return 0;
}

int sceIdStorageCreateLeaf(int key);

int prxIdStorageLookup(u16 key, u32 offset, void *buf, u32 len) {
	u32 k1 = pspSdkSetK1(0);
	memset(buf, 0, len);
	int ret = sceIdStorageLookup(key, offset, buf, len);
	pspSdkSetK1(k1);
	return ret;	   
}

int prxIdStorageReadLeaf(u16 key, void *buf) {
	u32 k1 = pspSdkSetK1(0);
	memset(buf, 0, 512);
	int ret = sceIdStorageReadLeaf(key, buf);
	pspSdkSetK1(k1);
	return ret;
}

int prxIdStorageWriteLeaf(u16 key, void *buf) {
	char tmp[512];
	u32 k1 = pspSdkSetK1(0);
	int ret = sceIdStorageReadLeaf(key, tmp);
	if (ret < 0)
		sceIdStorageCreateLeaf(key);
	ret = sceIdStorageWriteLeaf(key, buf);
	sceIdStorageFlush();
	pspSdkSetK1(k1);
	return ret;
}

int module_stop(void) {
	return 0;
}
