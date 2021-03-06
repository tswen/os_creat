/* bootpack?̃??C?? */

#include "bootpack.h"
#include <stdio.h>

#define KEYCMD_LED	0xed
void make_balloon8(unsigned char *buf, int xsize, int ysize, int bc);

void close_constask(struct TASK *task);
void close_console(struct SHEET *sht);

void HariMain(void)
{
	struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;
	struct SHTCTL *shtctl;
	char s[40];
	struct FIFO32 fifo, keycmd;
	int fifobuf[128], keycmd_buf[32];
	int mx, my, i, j, x, y, mmx = -1, mmy = -1, mmx2 = 0;
	int new_mx = -1, new_my = 0, new_wx = 0x7fffffff, new_wy = 0;
	unsigned int memtotal;
	struct MOUSE_DEC mdec;
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	unsigned char *buf_back, buf_mouse[256];
	struct SHEET *sht_back, *sht_mouse, *sht = 0, *key_win, *sht2;
	struct TASK *task_a, *task;
	static char keytable0[0x80] = {
		  0, 0x01, '1',  '2', '3', '4', '5', '6', '7', '8', '9', '0',  '-',  '^', 0x08, 0x09,
		'Q',  'W', 'E',  'R', 'T', 'Y', 'U', 'I', 'O', 'P', '@', '[', 0x0a,    0,  'A',  'S',
		'D',  'F', 'G',  'H', 'J', 'K', 'L', ';', ':',   0,   0, ']',  'Z',  'X',  'C',  'V',
		'B',  'N', 'M',  ',', '.', '/',   0, '*',   0, ' ',   0,   0,    0,    0,    0,    0,
		  0,    0,   0,    0,   0,   0,   0, '7', '8', '9', '-', '4',  '5',  '6',  '+',  '1',
		'2',  '3', '0',  '.',   0,   0,   0,   0,   0,   0,   0,   0,    0,    0,    0,    0,
		  0,    0,   0,    0,   0,   0,   0,   0,   0,   0,   0,   0,    0,    0,    0,    0,
		  0,    0,   0, 0x5c,   0,   0,   0,   0,   0,   0,   0,   0,    0, 0x5c,    0,    0
	};
	static char keytable1[0x80] = {
		0,   0,   '!', 0x22, '#', '$', '%', '&', 0x27, '(', ')', '~', '=', '~', 0x08, 0x09,
		'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '`', '{', 0x0a,   0,   'A', 'S',
		'D', 'F', 'G', 'H', 'J', 'K', 'L', '+', '*', 0,   0,   '}', 'Z', 'X', 'C', 'V',
		'B', 'N', 'M', '<', '>', '?', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1',
		'2', '3', '0', '.', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   '_', 0,   0,   0,   0,   0,   0,   0,   0,   0,   '|', 0,   0
	};
	int key_shift = 0, key_ctrl = 0, key_alt = 0;
	int key_leds = (binfo->leds >> 4) & 7, keycmd_wait = -1;
	int flag_e0 = 0, oldlang;
	int *fat;
	unsigned char *nihongo;
	struct FILEINFO *finfo;
	extern char hankaku[4096];

	unsigned char *buf_naomi, *buf_balloon;
	struct SHEET *sht_naomi, *sht_balloon;
	int flag_balloon = 0;

	static unsigned char naomi[26 * 28] = {
		0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x00,0x00,0x00,0x00,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,
		0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x00,0x00,0x00,0x03,0x03,0x03,0x03,0x00,0x00,0x00,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,
		0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x00,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x00,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,
		0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x00,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x00,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,
		0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x00,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x00,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,
		0x00,0x0e,0x0e,0x0e,0x0e,0x00,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x00,0x0e,0x0e,0x0e,0x0e,0x00,
		0x00,0x0e,0x0e,0x0e,0x00,0x03,0x03,0x00,0x03,0x03,0x03,0x03,0x00,0x03,0x03,0x00,0x03,0x03,0x03,0x03,0x00,0x03,0x03,0x00,0x0e,0x0e,0x0e,0x00,
		0x00,0x0e,0x0e,0x0e,0x00,0x03,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x03,0x00,0x0e,0x0e,0x0e,0x00,
		0x0e,0x00,0x0e,0x0e,0x00,0x03,0x03,0x00,0x03,0x03,0x03,0x03,0x00,0x03,0x03,0x00,0x03,0x03,0x03,0x03,0x00,0x03,0x03,0x00,0x0e,0x0e,0x0e,0x00,
		0x0e,0x00,0x0e,0x00,0x03,0x03,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x03,0x03,0x00,0x0e,0x00,0x0e,
		0x0e,0x00,0x0e,0x00,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x00,0x0e,0x00,0x0e,
		0x0e,0x0e,0x00,0x00,0x03,0x03,0x03,0x03,0x03,0x00,0x00,0x03,0x03,0x03,0x03,0x03,0x03,0x00,0x00,0x03,0x03,0x03,0x03,0x03,0x00,0x00,0x0e,0x0e,
		0x0e,0x0e,0x0e,0x00,0x03,0x03,0x03,0x03,0x03,0x00,0x00,0x03,0x03,0x03,0x03,0x03,0x03,0x00,0x00,0x03,0x03,0x03,0x03,0x03,0x00,0x0e,0x0e,0x0e,
		0x0e,0x0e,0x0e,0x00,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x00,0x00,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x00,0x0e,0x0e,0x0e,
		0x0e,0x0e,0x0e,0x0e,0x00,0x03,0x03,0x03,0x03,0x03,0x03,0x00,0x00,0x01,0x01,0x00,0x00,0x03,0x03,0x03,0x03,0x03,0x03,0x00,0x0e,0x0e,0x0e,0x0e,
		0x0e,0x0e,0x0e,0x0e,0x00,0x03,0x03,0x03,0x03,0x03,0x03,0x00,0x01,0x00,0x00,0x01,0x00,0x03,0x03,0x03,0x03,0x03,0x03,0x00,0x0e,0x0e,0x0e,0x0e,
		0x0e,0x0e,0x0e,0x0e,0x00,0x03,0x03,0x03,0x03,0x03,0x00,0x01,0x00,0x07,0x07,0x00,0x01,0x00,0x03,0x03,0x03,0x03,0x03,0x00,0x0e,0x0e,0x0e,0x0e,
		0x0e,0x0e,0x0e,0x0e,0x0e,0x00,0x03,0x03,0x03,0x03,0x00,0x01,0x00,0x07,0x07,0x00,0x01,0x00,0x03,0x03,0x03,0x03,0x00,0x0e,0x0e,0x0e,0x0e,0x0e,
		0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x00,0x03,0x03,0x03,0x03,0x00,0x01,0x00,0x00,0x01,0x00,0x03,0x03,0x03,0x03,0x00,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,
		0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x00,0x03,0x03,0x03,0x00,0x00,0x01,0x01,0x00,0x00,0x03,0x03,0x03,0x00,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,
		0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x00,0x03,0x03,0x03,0x03,0x00,0x00,0x03,0x03,0x03,0x03,0x00,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,
		0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x00,0x00,0x00,0x03,0x03,0x03,0x03,0x00,0x00,0x00,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,
		0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x00,0x00,0x0e,0x0e,0x00,0x00,0x00,0x00,0x0e,0x0e,0x00,0x00,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,
		0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x00,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x00,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,
		0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x00,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x00,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,
		0x0e,0x0e,0x0e,0x0e,0x0e,0x00,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x0e,0x00,0x0e,0x0e,0x0e,0x0e,0x0e
	};

	init_gdtidt();
	init_pic();
	io_sti(); /* IDT/PIC?̏????????I???????̂?CPU?̊??荞?݋֎~?????? */
	fifo32_init(&fifo, 128, fifobuf, 0);
	*((int *) 0x0fec) = (int) &fifo;
	init_pit();
	init_keyboard(&fifo, 256);
	enable_mouse(&fifo, 512, &mdec);
	io_out8(PIC0_IMR, 0xf8); /* PIT??PIC1?ƃL?[?{?[?h??????(11111000) */
	io_out8(PIC1_IMR, 0xef); /* ?}?E?X??????(11101111) */
	fifo32_init(&keycmd, 32, keycmd_buf, 0);

	memtotal = memtest(0x00400000, 0xbfffffff);
	memman_init(memman);
	memman_free(memman, 0x00001000, 0x0009e000); /*	0x00001000 - 0x0009efff */
	memman_free(memman, 0x00400000, memtotal - 0x00400000);
	*((int *) 0x0fe2) = (int) memtotal;

	init_timerctl();
	init_palette();
	shtctl = shtctl_init(memman, binfo->vram, binfo->scrnx, binfo->scrny);
	task_a = task_init(memman);
	fifo.task = task_a;
	task_run(task_a, 1, 2);
	*((int *) 0x0fe4) = (int) shtctl;

	/* sht_back */
	sht_back = sheet_alloc(shtctl);
	buf_back = (unsigned char *) memman_alloc_4k(memman, binfo->scrnx * binfo->scrny);
	sheet_setbuf(sht_back,  buf_back,  binfo->scrnx, binfo->scrny, -1);	/* ?????F?Ȃ? */
	init_screen8(buf_back, binfo->scrnx, binfo->scrny);
	sht_back->flags |= 0x01;	/* ?w?i */

	/* sht_naomi */
	sht_naomi = sheet_alloc(shtctl);
	buf_naomi = (unsigned char *) memman_alloc_4k(memman, 28 * 26);
	sheet_setbuf(sht_naomi, buf_naomi, 28, 26, COL8_008484);	/* ?????F???? */
	picdata8(buf_naomi, 28, naomi, 28, 26, 0, 0, 0);
	sht_naomi->flags |= 0x100;	/* ?????E?B???h?E */

	/* sht_balloon */
	sht_balloon = sheet_alloc(shtctl);
	buf_balloon = (unsigned char *) memman_alloc_4k(memman, 120 * 28);
	sheet_setbuf(sht_balloon, buf_balloon, 120, 28, COL8_008484);	/* ?????F???? */
	make_balloon8(buf_balloon, 120, 28, COL8_008484);
	putminifonts8_asc_sht(sht_balloon, 2, 5, COL8_000000, COL8_FFFFFF, "My name is naomisan", 19);
	sht_balloon->flags |= 0x100;	/* OSAkkie?????E?B???h?E */

	/* sht_cons */
	key_win = open_console(shtctl, memtotal);

	/* sht_mouse */
	sht_mouse = sheet_alloc(shtctl);
	sheet_setbuf(sht_mouse, buf_mouse, 16,  16, 99);	/* ?????F?ԍ???99 */
	init_mouse_cursor8(buf_mouse, 99/* ?w?i?F */);
	mx = (binfo->scrnx - 16) / 2; /* ???ʒ????ɂȂ??悤?ɍ??W?v?Z */
	my = (binfo->scrny - 16) / 2;

	sheet_slide(sht_back,  0,  0);
	sheet_slide(key_win,   8, 16);
	sheet_slide(sht_balloon, binfo->scrnx - 130, binfo->scrny - 70);
	sheet_slide(sht_naomi, binfo->scrnx - 50, binfo->scrny - 40);
	sheet_slide(sht_mouse, mx, my);
	sheet_updown(sht_back,  0);
	sheet_updown(key_win,   1);
	sheet_updown(sht_naomi, 2);
	sheet_updown(sht_mouse, 3);
	keywin_on(key_win);

	/* ?ŏ??̃L?[?{?[?h???Ԃ??ݒ肵?Ă??܂? */
	fifo32_put(&keycmd, KEYCMD_LED);
	fifo32_put(&keycmd, key_leds);

	/* nihongo.fnt?̓ǂݍ??? */
	fat = (int *) memman_alloc_4k(memman, 4 * 2880);
	file_readfat(fat, (unsigned char *) (ADR_DISKIMG + 0x000200));
	finfo = file_search("nihongo.fnt", (struct FILEINFO *) (ADR_DISKIMG + 0x002600), 224);
	if (finfo != 0) {
		i = finfo->size;
		nihongo = file_loadfile2(finfo->clustno, &i, fat);
		task_a->langmode = 1;
	} else {
		nihongo = (unsigned char *) memman_alloc_4k(memman, 16 * 256 + 32 * 94 * 47);
		task_a->langmode = 0;
		for (i = 0; i < 16 * 256; i++) {
			nihongo[i] = hankaku[i];	/* ?t?H???g???Ȃ??????̂Ŕ??p???????R?s?[ */
		}
		for (i = 16 * 256; i < 16 * 256 + 32 * 94 * 47; i++) {
			nihongo[i] = 0xff;	/* ?S?p??????0xff?Ŗ??ߐs???? */
		}
	}
	*((int *) 0x0fe8) = (int) nihongo;

	/* startup */
	finfo = file_search("osakkie.hsf", (struct FILEINFO *) (ADR_DISKIMG + 0x002600), 224);
	if (finfo != 0) {
		char *ss;
		i = finfo->size;
		ss = file_loadfile2(finfo->clustno, &i, fat);
		for (x = 0; x < i; x++) {
			if (ss[x] != 0x0d) {
				fifo32_put(&key_win->task->fifo, ss[x] + 256);
			}
		}
		memman_free_4k(memman, (int) ss, i);
	}

	memman_free_4k(memman, (int) fat, 4 * 2880);

	for (;;) {
		if (fifo32_status(&keycmd) > 0 && keycmd_wait < 0) {
			/* ?L?[?{?[?h?ɑ????f?[?^???????Α??? */
			keycmd_wait = fifo32_get(&keycmd);
			wait_KBC_sendready();
			io_out8(PORT_KEYDAT, keycmd_wait);
		}
		io_cli();
		if (fifo32_status(&fifo) == 0) {
			/* fifo???J???b?|?ɂȂ????̂ŁA?ۗ????Ă????`?悪?????Ύ??s */
			if (new_mx >= 0) {
				io_sti();
				sheet_slide(sht_mouse, new_mx, new_my);
				new_mx = -1;
			} else if (new_wx != 0x7fffffff) {
				io_sti();
				sheet_slide(sht, new_wx, new_wy);
				new_wx = 0x7fffffff;
			} else {
				task_sleep(task_a);
				io_sti();
			}
		} else {
			i = fifo32_get(&fifo);
			io_sti();
			if (key_win != 0 && key_win->flags == 0) {
				/* ???̓E?B???h?E???????ꂽ */
				if (shtctl->top == 2) {
					/* ?????}?E?X?Ɣw?i?????Ȃ? */
					key_win = 0;
				} else {
					key_win = shtctl->sheets[shtctl->top - 2];
					keywin_on(key_win);
				}
			}
			if (256 <= i && i < 512) {
				/* ?L?[?{?[?h */
				i -= 256;	// ???Ɉ????Ƃ?
				if (i == 0xe0) {
					/* E0?g???L?[ */
					flag_e0 = 1;
				}
				if (i < 0x80 && flag_e0 == 0) {
					/* ?L?[?R?[?h?𕶎??R?[?h?ɕϊ? */
					if (key_shift == 0)
						s[0] = keytable0[i];
					else
						s[0] = keytable1[i];
				} else {
					s[0] = 0;
				}
				if ('A' <= s[0] && s[0] <= 'Z')	{
					/* ?A???t?@?x?b?g?????͂??ꂽ */
					if (((key_leds & 4) == 0 && key_shift == 0) ||
						((key_leds & 4) != 0 && key_shift != 0)   ) {
						s[0] += 0x20;	/* ???????ɕϊ? */
					}
				}
				if (s[0] != 0 && key_win != 0) {
					/* ???ʂ̕???, Enter, BackSpace */
					if (key_ctrl == 0) {
						/* ?R???\?[???? */
						fifo32_put(&key_win->task->fifo, s[0] + 256);
					}
				}
				if (i == 0x57 && shtctl->top > 2) {
					/* F11?L?[ */
					sheet_updown(shtctl->sheets[1], shtctl->top - 2);
				}
				if (i == 0x2a)
					key_shift |= 1;			// ???V?t?gon
				if (i == 0x36)
					key_shift |= 2;			// ?E?V?t?gon
				if (i == 0xaa)
					key_shift &= ~1;		// ???V?t?goff
				if (i == 0xb6)
					key_shift &= ~2;		// ?E?V?t?goff
				if (i == 0x1d) {
					if (flag_e0 == 0) {
						key_ctrl |= 1;		// ??CTRLon
					} else {
						key_ctrl |= 2;		// ?ECTRLon
					}
				}
				if (i == 0x9d) {
					if (flag_e0 == 0) {
						key_ctrl &= ~1;		// ??CTRLoff
					} else {
						key_ctrl &= ~2;		// ?ECTRLoff
					}
				}
				if (i == 0x38) {
					if (flag_e0 == 0) {
						key_alt |= 1;		// ??ALTon
					} else {
						key_alt |= 2;		// ?EALTon
					}
				}
				if (i == 0xb8) {
					if (flag_e0 == 0) {
						key_alt &= ~1;		// ??ALToff
					} else {
						key_alt &= ~2;		// ?EALToff
					}
				}
				if (i == 0x3a) {
					/* CapsLock */
					key_leds ^= 4;
					fifo32_put(&keycmd, KEYCMD_LED);
					fifo32_put(&keycmd, key_leds);
				}
				if (i == 0x45) {
					/* NumLock */
					key_leds ^= 2;
					fifo32_put(&keycmd, KEYCMD_LED);
					fifo32_put(&keycmd, key_leds);
				}
				if (i == 0x46) {
					/* ScrollLock */
					key_leds ^= 1;
					fifo32_put(&keycmd, KEYCMD_LED);
					fifo32_put(&keycmd, key_leds);
				}
				if ((i == 0x48 || i == 0x4b || i == 0x4d || i == 0x50) && key_win != 0) {
					/* ?J?[?\???L?[ */
					/* ???݂͂܂Ƃ߂Ă??邪?A???????????K?v?????o?Ă????番???? */
					if (flag_e0 == 1) {
						fifo32_put(&key_win->task->fifo, 0xe0 + 256);
						fifo32_put(&key_win->task->fifo, i + 256);
					}
				}
				if ((i == 0x49 || i == 0x51) && key_win != 0) {
					/* PageUp,Down?L?[ */
					/* ???݂͂܂Ƃ߂Ă??邪?A???????????K?v?????o?Ă????番???? */
					if (flag_e0 == 1) {
						fifo32_put(&key_win->task->fifo, 0xe0 + 256);
						fifo32_put(&key_win->task->fifo, i + 256);
					}
				}
				if (((i == 0x0f && key_alt != 0) || i == 0x43) && key_win != 0) {
					/* Tab ???? Alt + Tab or F9 */
					keywin_off(key_win);
					j = key_win->height - 1;
					if (j == 0) {
						j = shtctl->top - 2;
					}
					key_win = shtctl->sheets[j];
					keywin_on(key_win);
				}
				if (i == 0x2e && key_ctrl != 0 && key_win != 0) {
					/* Shift + F1 ???? Ctrl + C */
					task = key_win->task;
					if (task != 0 && task->tss.ss0 != 0) {
						task->langmode = task->langmode0;	/* API?ŕύX????langmode?????ɖ߂? */
						oldlang = task_a->langmode;		/* task_a??langmode???ꎞ?I?Ƀ^?X?N?ɂ??킹?? */
						task_a->langmode = task->langmode;
						cons_putstr0(task->cons, "\nBreak(key) :\n");
						task_a->langmode = oldlang;
						io_cli();
						task->tss.eax = (int) &(task->tss.esp0);
						task->tss.eip = (int) asm_end_app;
						io_sti();
						task_run(task, -1, 0);
					}
				}
				if (i == 0x31 && key_ctrl != 0) {
					/* Shift + F2 ???? Ctrl + N */
					/* ?V???????????R???\?[?????A?N?e?B?u?? */
					keywin_off(key_win);
					key_win = open_console(shtctl, memtotal);
					sheet_slide(key_win, 8, 16);
					sheet_updown(key_win, shtctl->top - 1);
					keywin_on(key_win);
				}
				if (i != 0xe0 && flag_e0 == 1) {
					/* ???̑?e0?g???L?[ */
					if ((i == 0x1c || i == 0x35) && key_win != 0) {
						/* ?e???L?[?n?i?Ƃ肠?????????ŗǂ??j */
						fifo32_put(&key_win->task->fifo, keytable0[i] + 256);
					}
					/* e0?g???L?[?g?p?ς? */
					flag_e0 = 0;
				}
				if (i == 0xfa) {
					/* ?L?[?{?[?h???f?[?^???󂯎????? */
					keycmd_wait = -1;
				}
				if (i == 0xfe) {
					/* ?L?[?{?[?h???f?[?^???󂯎????Ȃ????? */
					wait_KBC_sendready();
					io_out8(PORT_KEYDAT, keycmd_wait);
				}
			} else if (512 <= i && i < 768) {
				/* ?}?E?X */
				if (mouse_decode(&mdec, i - 512)) {
					/* ?}?E?X?ړ? */
					mx += mdec.x;
					my += mdec.y;
					if (mx < 0)
						mx = 0;
					if (my < 0)
						my = 0;
					if (mx > binfo->scrnx - 1)
						mx = binfo->scrnx - 1;
					if (my > binfo->scrny - 1)
						my = binfo->scrny - 1;
					new_mx = mx;
					new_my = my;
					if (mdec.btn & 0x01) {
						/* ???{?^?? */
						if (mmx < 0) {
							/* ?ʏ탂?[?h */
							/* ???̉??~?????珇?Ƀ}?E?X???????V?[?g???T?? */
							for (j = shtctl->top - 1; j > 0; j--) {
								sht = shtctl->sheets[j];
								x = mx - sht->vx0;
								y = my - sht->vy0;
								if (0 <= x && x < sht->bxsize && 0 <= y && y < sht->bysize) {
									if (sht->buf[y * sht->bxsize + x] != sht->col_inv) {
										if ((sht->flags & 0x100) == 0) {
											/* ?i?I?~?????ȊO */
											sheet_updown(sht_balloon, -1);	/* ?܂??o???[???͌????ɍs???Ă??炤 */
											flag_balloon = 0;
											sheet_updown(sht, shtctl->top - 2);
											/* ?E?B???h?E?Ƃ? */
											if (sht != key_win) {
												keywin_off(key_win);
												key_win = sht;
												keywin_on(key_win);
											}
											/* ???ʃE?B???h?E */
											if (3 <= x && x < sht->bxsize - 3 && 3 <= y && y < 21) {
												mmx = mx;	/* ?E?B???h?E?ړ????[?h?? */
												mmy = my;
												mmx2 = sht->vx0;
												new_wy = sht->vy0;
											}
											if (sht->bxsize - 20 <= x && x < sht->bxsize - 5 && 5 <= y && y < 19) {
												/* [X]?{?^???N???b?N */
												if ((sht->flags & 0x10) != 0) {
													/* ?A?v???????????E?B???h?E???H */
													task = sht->task;
													task->langmode = task->langmode0;	/* API?ŕύX????langmode?????ɖ߂? */
													oldlang = task_a->langmode;		/* task_a??langmode???ꎞ?I?Ƀ^?X?N?ɂ??킹?? */
													task_a->langmode = task->langmode;
													cons_putstr0(task->cons, "\nBreak(mouse) :\n");
													task_a->langmode = oldlang;
													io_cli();
													task->tss.eax = (int) &(task->tss.esp0);
													task->tss.eip = (int) asm_end_app;
													io_sti();
													task_run(task, -1, 0);
												} else {
													/* Console */
													task = sht->task;
													sheet_updown(sht, -1);	/* ?Ƃ肠???????\???? */
													keywin_off(key_win);
													key_win = 0;
													if (shtctl->top > 2) {
														key_win = shtctl->sheets[shtctl->top - 2];
														keywin_on(key_win);
													}
												}
											}
										} else {
											/* ?i?I?~???񂩃X?^?[?g???j???[ */
											if (sht == sht_naomi) {
												if (flag_balloon == 0) {
													flag_balloon = 1;
													sheet_updown(sht_balloon, shtctl->top - 1);
												} else {
													flag_balloon = 0;
													sheet_updown(sht_balloon, -1);
												}
											}
										}
										break;
									}
								}
							}
						} else {
							/* ?E?B???h?E?ړ????[?h */
							x = mx - mmx;	/* ?ړ??ʂ??v?Z */
							y = my - mmy;
							new_wx = (mmx2 + x + 2) & ~3;
							new_wy = new_wy + y;
							mmy = my;		/* ?ړ????̍??W?ɍX?V */
						}
					} else if (mdec.btn & 0x02) {
						/* ?i?I?~?????ړ??\?? */
					} else {
						mmx = -1;	/* ?ʏ탂?[?h */
						if (new_wx != 0x7fffffff) { 
							sheet_slide(sht, new_wx, new_wy);	/* ???x?m?肳???? */
							new_wx = 0x7fffffff;
						}
					}
				}
			} else if (768 <= i && i < 1024) {
				close_console(shtctl->sheets0 + (i - 768));
			} else if (1024 <= i && i < 2024) {
				close_constask(taskctl->tasks0 + (i - 1024));
			} else if (2024 <= i && i < 2280) {
				/* ?R???\?[?????????? */
				sht2 = shtctl->sheets0 + (i - 2024);
				memman_free_4k(memman, (int) sht2->buf, 256 * 165);
				sheet_free(sht2);
			} else if (i == 0x4000) {
				/* key_win?ύX?v?? from api.c */
				key_win = shtctl->sheets[shtctl->top - 2];
			}
		}
	}
}

void keywin_off(struct SHEET *key_win)
{
	change_wtitle8(key_win, 0);
	if ((key_win->flags & 0x20) != 0) {
		fifo32_put(&key_win->task->fifo, 3);	/* ?R???\?[???̃J?[?\??OFF */
	}
	return;
}

void keywin_on(struct SHEET *key_win)
{
	change_wtitle8(key_win, 1);
	if ((key_win->flags & 0x20) != 0) {
		fifo32_put(&key_win->task->fifo, 2);	/* ?R???\?[???̃J?[?\??ON */
	}
	return;
}

struct TASK *open_constask(struct SHEET *sht, unsigned int memtotal)
{
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct TASK *task = task_alloc();
	int *cons_fifo = (int *) memman_alloc_4k(memman, 128 * 4);
	task->cons_stack = memman_alloc_4k(memman, 64 * 1024);
	task->tss.esp = task->cons_stack + 64 * 1024 - 12;
	task->tss.eip = (int) &console_task;
	task->tss.es = 1 * 8;
	task->tss.cs = 2 * 8;
	task->tss.ss = 1 * 8;
	task->tss.ds = 1 * 8;
	task->tss.fs = 1 * 8;
	task->tss.gs = 1 * 8;
	*((int *) (task->tss.esp + 4)) = (int) sht;
	*((int *) (task->tss.esp + 8)) = memtotal;
	task_run(task, 2, 2);
	fifo32_init(&task->fifo, 128, cons_fifo, task);
	return task;
}

struct SHEET *open_console(struct SHTCTL *shtctl, unsigned int memtotal)
{
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct SHEET *sht = sheet_alloc(shtctl);
	unsigned char *buf = (unsigned char *) memman_alloc_4k(memman, 256 * 165);
	sheet_setbuf(sht, buf, 256, 165, -1);	/* ?????F?Ȃ? */
	make_window8(buf, 256, 165, "Console", 0/* ?i?I?~???? */, 0);
	make_textbox8(sht, 8, 28, 240, 128, COL8_000000);
	sht->task = open_constask(sht, memtotal);
	sht->flags |= 0x20;	/* ?J?[?\?????? */
	return sht;
}

void close_constask(struct TASK *task)
{
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	task_sleep(task);
	memman_free_4k(memman, task->cons_stack, 64 * 1024);
	memman_free_4k(memman, (int) task->fifo.buf, 128 * 4);
	task->flags = 0;	/* task_free(task);?̑????? */
	
}

void close_console(struct SHEET *sht)
{
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct TASK *task = sht->task;
	memman_free_4k(memman, (int) sht->buf, 256 * 165);
	sheet_free(sht);
	close_constask(task);
	return;
}
