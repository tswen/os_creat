#include "apilib.h"

void HariMain(void)
{
	char buf[160 * 100], naomi3[3];
	int win, i, x, y;

	naomi3[0] = 0x80;
	naomi3[1] = 0x81;
	naomi3[2] = 0;

	tomo_setlang(0);
	win = api_openwin(buf, 160, 100, -1, "walk");
	api_boxfilwin(win, 4, 24, 155, 95, 0 /*  */);
	x = 76;
	y = 56;
	api_putstrwin(win, x, y, 3 /* © */, 2, naomi3);
	for (;;) {
		i = api_getkey(1);
		api_putstrwin(win, x, y, 0 /*  */, 2, naomi3); /* ΕΑ· */
		if (i == '4' && x >   4) { x -= 8; }
		if (i == '6' && x < 140) { x += 8; }
		if (i == '8' && y >  24) { y -= 8; }
		if (i == '2' && y <  80) { y += 8; }
		if (i == 0x0a) { break; } /* EnterΕIΉ */
		api_putstrwin(win, x, y, 3 /* © */, 2, naomi3);
	}	
	api_closewin(win);
	api_end();
}
