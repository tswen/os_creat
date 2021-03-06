/* picture.c */
#include "bootpack.h"

int picture_info(struct DLL_STRPICENV *env, int *info, int size, char *fp)
{
	/* 自動判別 */
	if (info_BMP(env, info, size, fp) == 0) {
		/* BMPではなかった */
		if (info_JPEG(env, info, size, fp) == 0) {
			/* JPEGでもなかった */
			if (info_ICO(env, info, size, fp) == 0) {
				/* ICOでもなかった */
				return 0;
			}
		}
	}
	return 1;
}

int picture_decode0(int mode, struct DLL_STRPICENV *env, int size, char *fp, int b_type, char *buf, int skip)
{
	int i = 1;
	if (mode == 1) {
		i = decode0_BMP (env, size, fp, b_type, (char *) buf, skip);
	} else if (mode == 2) {
		i = decode0_JPEG(env, size, fp, b_type, (char *) buf, skip);
	} else if (mode == 4) {
		i = decode0_ICO (env, size, fp, b_type, (char *) buf, skip);
	}
	return i;
}

unsigned char rgb2pal(int r, int g, int b, int x, int y)
{
	static int table[4] = { 3, 1, 0, 2 };
	int i;
	x &= 1; /* 偶数か奇数か */
	y &= 1;
	i = table[x + y * 2];	/* 中間色を作るための定数 */
	r = (r * 21) / 256;	/* これで 0〜20 になる */
	g = (g * 21) / 256;
	b = (b * 21) / 256;
	r = (r + i) / 4;	/* これで 0〜5 になる */
	g = (g + i) / 4;
	b = (b + i) / 4;
	return 16 + r + g * 6 + b * 36;
}
