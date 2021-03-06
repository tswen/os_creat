#include "apilib.h"

struct SYSINFO {
	int scr_x;
	int scr_y;
	int scr_c;
};

unsigned short rgb2pal(int r, int g, int b, int x, int y, int cb);

void HariMain(void)
{
	short *buf;
	int win, x, y;
	struct SYSINFO sysinfo;
	api_sysinfo(&sysinfo);
	api_initmalloc();
	buf = api_malloc(144 * 164 * 2);
	win = api_openwin(buf, 144, 164, -1, "color2");
	for (y = 0; y < 128; y++) {
		for (x = 0; x < 128; x++) {
			buf[(x + 8) + (y + 28) * 144] = rgb2pal(x * 2, y * 2, 0, x, y, sysinfo.scr_c);
		}
	}
	api_refreshwin(win, 8, 28, 136, 156);
	api_getkey(1); /* てきとうなキー入力を待つ */
	api_end();
}

unsigned short rgb2pal(int r, int g, int b, int x, int y, int cb)
{
	if (cb == 8) {
		static	int table[4] = { 3, 1, 0, 2 };
		int		i;
		x &= 1;					/* 偶数か奇数か */
		y &= 1;
		i = table[x + y * 2];	/* 中間色を作るための定数 */
		r = (r * 21) / 256;		/* これで 0〜20 になる */
		g = (g * 21) / 256;
		b = (b * 21) / 256;
		r = (r +  i) /   4;		/* これで 0〜5 になる */
		g = (g +  i) /   4;
		b = (b +  i) /   4;
		return((unsigned short) (16 + r + g * 6 + b * 36));
	} else {
		return((unsigned short) (((r << 8) & 0xf800) | ((g << 3) & 0x07e0) | (b >> 3)));
	}
}
