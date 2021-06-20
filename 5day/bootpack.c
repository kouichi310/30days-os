#include <stdio.h>

void io_hlt(void);
void io_cli(void);
void io_out8(int port, int data);
int io_load_eflags(void);
void io_store_eflags(int eflags);

void init_palette(void);
void set_palette(int start, int end, unsigned char *rgb);
void boxfill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1);
void init_screen(char *vram, int x, int y);
void putfont8(char *vram, int xsize, int x, int y, char c, char *font);
void putimage(char *vram, int xsize, int x, int y, char c, char image[]);
void putfonts8_asc(char *vram, int xsize, int x, int y, char c, unsigned char *s);

#define COL8_000000		0
#define COL8_FF0000		1
#define COL8_00FF00		2
#define COL8_FFFF00		3
#define COL8_0000FF		4
#define COL8_FF00FF		5
#define COL8_00FFFF		6
#define COL8_FFFFFF		7
#define COL8_C6C6C6		8
#define COL8_840000		9
#define COL8_008400		10
#define COL8_848400		11
#define COL8_000084		12
#define COL8_840084		13
#define COL8_008484		14
#define COL8_848484		15

struct BOOTINFO {
	char cyls, leds, vmode, reserve;
	short scrnx, scrny;
	char *vram;
};

void HariMain(void)
{
	struct BOOTINFO *binfo = (struct BOOTINFO *) 0x0ff0;
	char image[]="SDVBVFVBS8SDVBVFVBS8SDVBVFVBS8SDVBVFVBS8SBVFVBT111111BS8VCVBVDT111111BVCV8VCVBVDT111111BVCV8VBTBVBV111111BVBT111111BVBTBV8VBTBVFVBVH1111111IVBTBV8VBTBVFVBVH1111111IVBTBV8VBTBVBT111111BTFVBTBV8VBTBVBVBV111111BTFVBTBV8VBTBVBVBV111111BTFVBTBV8VBTBVBVBV111111BTFVBTBV8VBTBVBVBV111111BT111111BVBTBV8VBTBVJVFVBVBTBV8VBTBVJVFVBVBTBV8VBTBVBVDT111111BVBTBV8VCVBVCV111111FVCV8VCVBVCV111111FVCV8SBVBVBVBVV1111BS8SBVBVBVB1111111BVBS8SBVBVBVB1111111BVBS8SBVBVBVB1111111BVBS8SBTBVB1111111BVBS8AV1111111BVB1111111BV8AV1111111BVB1111111BV8TV11111BSV1111BV1111111BVBVB11111118TV11111BT111111BTIVBVBVB11111118TV11111BT111111BTIVBVBVB11111118TV11111BT111111BTIVBVBVB11111118KV1GVIVBVIVBVI11111118KV1GVIVBVIVBVI11111118KV1GVIVBVIVBVI11111118LS111111BTVV111BTBVBV1111118LTVV111BVBVBT111111BV111111HVBV1111118LTVV111BVBVBT111111BV111111HVBV1111118SV11111BTVV111BTBVBV1111118VBTFV1111111FV1111111B1111111HVBVDV8VBTFV1111111FV1111111B1111111HVBVDV8TV11111BSS1111BT8LVBV111111BVBTBSV1111BT8LVBV111111BVBTBSV1111BT8LVBV111111BVBTBSV1111BT8SSST18T111111BVBVIVH1111111BVHVBVB11111118T111111BVBVIVH1111111BVHVBVB11111118T111111BTIVBT111111HTBV1111118VDTCVBV111111HTBV1111118VDTCVBV111111HTBV1111118VDTCVBV111111HTBV1111118VDTHVBSVV111BV1111118VJVHVBVBTT1111IV8VJVHVBVBTT1111IV8VIVNTV111111BSV1111BVBV8VIVNT1111111HSV1111BV8VIVNT1111111HSV1111BV8VIVNSSVV118EVBV111111BVBU8EVBV111111BVBU8EVBV111111BVBU8SBVBT111111BV111111BVBTV111118SBVBVBVGVBVBVBT8SBVBVBVGVBVBVBT8SBVBTB1111111BVBVBTV111118VCVDV1111111B1111111BVIV1111111B11111118VCVDV1111111B1111111BVIV1111111B11111118VBTBVBSSV118VBTBVBTFS111111BV1111118VBTBVBTFS111111BV1111118VBTBVBTFS111111BV1111118VBTBVBTFSTV1118VBTBVBVBVIV111111BV111111BTV111118VBTBVBVBVIV111111BV111111BTV111118VBTBVBVBVFTBV111111BTV111118VBTBVBVDVIVIV111111BV8VBTBVBVDVIVIV111111BV8VBTBVBT111111BVFSBV8VCVBT111111DTVV1111FV8VCVBT111111DTVV1111FV8VCVBT111111DTVV1111FV8SBTT1111BSV111118SBVBTV1111IS8SBVBTV1111IS8SBVBTV1111IS8";
	init_palette();
	init_screen(binfo->vram, binfo->scrnx, binfo->scrny);
	putfonts8_asc(binfo->vram, binfo->scrnx,  10, 10, COL8_FFFFFF, "Hello, world!");
	putfonts8_asc(binfo->vram, binfo->scrnx, 30, 31, COL8_FFFFFF, "My Twitter name is");
	putfonts8_asc(binfo->vram, binfo->scrnx, 30, 80, COL8_00FF00, "@siroRabi_jk");
	putfonts8_asc(binfo->vram, binfo->scrnx, 31, 80, COL8_00FF00, "@siroRabi_jk");
	putfonts8_asc(binfo->vram, binfo->scrnx, 10, 120, COL8_FFFFFF, "Please follow me!");
	putfonts8_asc(binfo->vram, binfo->scrnx, 11, 120, COL8_FFFFFF, "Please follow me!");
	putimage(binfo->vram, binfo->scrnx, 150, 70, COL8_FFFFFF,image);

	
	for (;;) {
		io_hlt();
	}
}

void init_palette(void)
{
	static unsigned char table_rgb[16 * 3] = {
		0x00, 0x00, 0x00,	/*  0:黒 */
		0xff, 0x00, 0x00,	/*  1:明るい赤 */
		0x00, 0xff, 0x00,	/*  2:明るい緑 */
		0xff, 0xff, 0x00,	/*  3:明るい黄色 */
		0x00, 0x00, 0xff,	/*  4:明るい青 */
		0xff, 0x00, 0xff,	/*  5:明るい紫 */
		0x00, 0xff, 0xff,	/*  6:明るい水色 */
		0xff, 0xff, 0xff,	/*  7:白 */
		0xc6, 0xc6, 0xc6,	/*  8:明るい灰色 */
		0x84, 0x00, 0x00,	/*  9:暗い赤 */
		0x00, 0x84, 0x00,	/* 10:暗い緑 */
		0x84, 0x84, 0x00,	/* 11:暗い黄色 */
		0x00, 0x00, 0x84,	/* 12:暗い青 */
		0x84, 0x00, 0x84,	/* 13:暗い紫 */
		0x00, 0x84, 0x84,	/* 14:暗い水色 */
		0x84, 0x84, 0x84	/* 15:暗い灰色 */
	};
	set_palette(0, 15, table_rgb);
	return;

	/* static char 命令は、データにしか使えないけどDB命令相当 */
}

void set_palette(int start, int end, unsigned char *rgb)
{
	int i, eflags;
	eflags = io_load_eflags();	/* 割り込み許可フラグの値を記録する */
	io_cli(); 					/* 許可フラグを0にして割り込み禁止にする */
	io_out8(0x03c8, start);
	for (i = start; i <= end; i++) {
		io_out8(0x03c9, rgb[0] / 4);
		io_out8(0x03c9, rgb[1] / 4);
		io_out8(0x03c9, rgb[2] / 4);
		rgb += 3;
	}
	io_store_eflags(eflags);	/* 割り込み許可フラグを元に戻す */
	return;
}

void boxfill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1)
{
	int x, y;
	for (y = y0; y <= y1; y++) {
		for (x = x0; x <= x1; x++)
			vram[y * xsize + x] = c;
	}
	return;
}

void init_screen(char *vram, int x, int y)
{
	boxfill8(vram, x, COL8_008484,  0,     0,      x -  1, y - 29);
	boxfill8(vram, x, COL8_C6C6C6,  0,     y - 28, x -  1, y - 28);
	boxfill8(vram, x, COL8_FFFFFF,  0,     y - 27, x -  1, y - 27);
	boxfill8(vram, x, COL8_C6C6C6,  0,     y - 26, x -  1, y -  1);

	boxfill8(vram, x, COL8_FFFFFF,  3,     y - 24, 59,     y - 24);
	boxfill8(vram, x, COL8_FFFFFF,  2,     y - 24,  2,     y -  4);
	boxfill8(vram, x, COL8_848484,  3,     y -  4, 59,     y -  4);
	boxfill8(vram, x, COL8_848484, 59,     y - 23, 59,     y -  5);
	boxfill8(vram, x, COL8_000000,  2,     y -  3, 59,     y -  3);
	boxfill8(vram, x, COL8_000000, 60,     y - 24, 60,     y -  3);

	boxfill8(vram, x, COL8_848484, x - 47, y - 24, x -  4, y - 24);
	boxfill8(vram, x, COL8_848484, x - 47, y - 23, x - 47, y -  4);
	boxfill8(vram, x, COL8_FFFFFF, x - 47, y -  3, x -  4, y -  3);
	boxfill8(vram, x, COL8_FFFFFF, x -  3, y - 24, x -  3, y -  3);
	return;
}

void putfont8(char *vram, int xsize, int x, int y, char c, char *font)
{
	int i;
	char *p, d /* data */;
	for (i = 0; i < 16; i++) {
		p = vram + (y + i) * xsize + x;
		d = font[i];
		if ((d & 0x80) != 0) { p[0] = c; }
		if ((d & 0x40) != 0) { p[1] = c; }
		if ((d & 0x20) != 0) { p[2] = c; }
		if ((d & 0x10) != 0) { p[3] = c; }
		if ((d & 0x08) != 0) { p[4] = c; }
		if ((d & 0x04) != 0) { p[5] = c; }
		if ((d & 0x02) != 0) { p[6] = c; }
		if ((d & 0x01) != 0) { p[7] = c; }
	}
	return;
}

void putfonts8_asc(char *vram, int xsize, int x, int y, char c, unsigned char *s)
{
	extern char hankaku[4096];
	for (; *s != 0x00; s++) {
		putfont8(vram, xsize, x, y, c, hankaku + *s * 16);
		x += 8;
	}
	return;
}

void putimage(char *vram, int xsize, int x, int y, char c, char image[])
{
	int i=0,j=0,k=0,l=0;
	char *p, d /* data */;
	for (i = 0;image[i]!='\0'; i++) {
		p = vram + (y + j) * xsize + x;
		d = image[i];
		if(d=='A')k+=58;
		if(d=='B')k+=5;
		if(d=='C')k+=31;
		if(d=='D')k+=24;
		if(d=='E')k+=52;
		if(d=='F')k+=11;
		if(d=='G')k+=17;
		if(d=='H')k+=12;
		if(d=='I')k+=18;
		if(d=='J')k+=37;
		if(d=='K')k+=19;
		if(d=='L')k+=7;
		if(d=='N')k+=4;
		if(d=='S'){
			for(l=0;l<47;l++)p[l+k]=c;
			k+=47;
		}
		if(d=='T'){
			for(l=0;l<21;l++)p[l+k]=c;
			k+=21;
		}
		if(d=='U'){
			for(l=0;l<30;l++)p[l+k]=c;
			k+=30;
		}
		if(d=='V'){
			for(l=0;l<8;l++)p[l+k]=c;
			k+=8;
		}
		if (d=='1'){
			p[k]=c;
			k++;
		}
		if (d=='8'){
			k=0;
			j++;
		}
		
	}
	return;
}
