#include <pspsdk.h>
#include <pspkernel.h>
#include <pspidstorage.h>
#include <string.h> // memset()

PSP_MODULE_INFO("regedit_prx", 0x1006, 1, 0);
PSP_MAIN_THREAD_ATTR(0);

int sceIdStorageCreateLeaf(int key);
int sceIdStorageDeleteLeaf(int key);

int module_start(SceSize args, void *argp)
{
	return 0;
}

int module_stop(void)
{
	return 0;
}

u32 Scramble(void) // raing3
{
	int k1 = pspSdkSetK1(0);

	u32 scramb;
	u32 buf[4];
	u32 sha[5];

	buf[0] = *(vu32*)(0xBC100090);
	buf[1] = *(vu32*)(0xBC100094);
	buf[2] = *(vu32*)(0xBC100090)<<1;
	buf[3] = 0xD41D8CD9;

	sceKernelUtilsSha1Digest((u8*)buf, sizeof(buf), (u8*)sha);

	scramb = (sha[0] ^ sha[3]) + sha[2];

	pspSdkSetK1(k1);

	return scramb;
}

int NewKey(int key)
{
	int ret;
	u32 k1;

	k1 = pspSdkSetK1(0);
		ret = sceIdStorageCreateLeaf(key);
		sceIdStorageFlush();
	pspSdkSetK1(k1);

	return ret;
}

int ReadKey(int key, char*buf)
{
	int ret;
	u32 k1;

	k1 = pspSdkSetK1(0);
		memset(buf, 0, 512);
		ret = sceIdStorageReadLeaf(key, buf);
	pspSdkSetK1(k1);

	return ret;
}

int WriteKey(int key, char*buf)
{
	int ret;
	u32 k1;
	char tmp[512];

	k1 = pspSdkSetK1(0);
		ret = sceIdStorageReadLeaf(key, tmp);
		if (ret < 0)
			sceIdStorageCreateLeaf(key); // если ключа нет
		ret = sceIdStorageWriteLeaf(key, buf);
		sceIdStorageFlush();
	pspSdkSetK1(k1);

	return ret;
}

int DeleteKey(int key)
{
	int ret;
	u32 k1;

	k1 = pspSdkSetK1(0);
		ret = sceIdStorageDeleteLeaf(key);
		sceIdStorageFlush();
	pspSdkSetK1(k1);

	return ret;
}
