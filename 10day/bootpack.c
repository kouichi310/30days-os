/* bootpack�̃��C�� */

#include "bootpack.h"
#include <stdio.h>

void HariMain(void)
{
	struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;
	char s[40], keybuf[32], mousebuf[128];
	int mx, my, i;
	unsigned int memtotal;
	struct MOUSE_DEC mdec;
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct SHTCTL *shtctl;
	struct SHEET *sht_back, *sht_mouse;
	unsigned char *buf_back, buf_mouse[256];

	init_gdtidt();
	init_pic();
	io_sti(); /* IDT/PIC�̏��������I������̂�CPU�̊��荞�݋֎~������ */
	fifo8_init(&keyfifo, 32, keybuf);
	fifo8_init(&mousefifo, 128, mousebuf);
	io_out8(PIC0_IMR, 0xf9); /* PIC1�ƃL�[�{�[�h������(11111001) */
	io_out8(PIC1_IMR, 0xef); /* �}�E�X������(11101111) */

	init_keyboard();
	enable_mouse(&mdec);
	memtotal = memtest(0x00400000, 0xbfffffff);
	memman_init(memman);
	memman_free(memman, 0x00001000, 0x0009e000); /* 0x00001000 - 0x0009efff */
	memman_free(memman, 0x00400000, memtotal - 0x00400000);

	int serial = init_serial();
	debug("hariboteos\nHello, World\n");

	unsigned char *ethernet_buf  = (unsigned char *) memman_alloc_4k(memman, ETHERNET_BUFFER_LEN);
	unsigned char *ip_buf  = (unsigned char *) memman_alloc_4k(memman, IP_BUFFER_LEN);
	init_nic();
	init_ethernet(ethernet_buf);
	init_ip(ip_buf);

	struct IP packet = {0};
	unsigned char data[] = {
						0x08,0x00,0x4d,0x4d,0x00,0x01,0x00,0x0e,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69
					};
	unsigned char dst[] = {0x0a,0x00,0x02,0x02};
	unsigned char src[] = {0x0a,0x00,0x02,0x0f};
	packet.version = 0x04;
	packet.ihl = DEFAULT_HEADER_LEN;
	packet.tos = 0x00;
	packet.len = (DEFAULT_HEADER_LEN*4+sizeof(data) << 8) | (DEFAULT_HEADER_LEN*4+sizeof(data) >> 8);
	packet.identification = 0;
	packet.flags = 0;
	packet.flaggment_offset = 0;
	packet.ttl = 0x34;
	packet.protocol = 0x01; //icmp
	packet.checksum = 0; //todo
	packet.payload_len = sizeof(data);
	memcpy(packet.src,src,IP_ADDR_LEN);
	memcpy(packet.dst,dst,IP_ADDR_LEN);
	memcpy(packet.payload,data,sizeof(data));

	unsigned short checksum = get_checksum(packet);
	packet.checksum = checksum;
	
	ip_send(packet);

	while(1){
		//ethernet_receive();
		//if(dump_frame() > 0)debug("\n");
	}

	// unsigned char dst[] = {0x52,0x55,0x0a,0x00,0x02,0x02};
	// unsigned char src[] = {0x52,0x54,0x00,0x12,0x34,0x56};
	// unsigned char type[] = {0x08,0x06};
	// unsigned char data[] = {
	// 						0x00,0x01,0x08,0x00,0x06,0x04,0x00,0x01,0x52,0x54,0x00,0x12,0x34,0x56,0x0a,0x00,0x02,0x0f,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x0a,0x00,0x02,0x02
	// 					};
	// unsigned short len = sizeof(data);

	// struct ETHERNET frame;
	// memcpy(frame.dst,dst,MAC_ADDR_LEN);
	// memcpy(frame.src,src,MAC_ADDR_LEN);
	// memcpy(frame.type,type,ETHERNET_TYPE_LEN);
	// memcpy(frame.payload,data,len);
	// frame.len = len;

	// ethernet_send(frame);

	// while(1){
	// }

	init_palette();
	shtctl = shtctl_init(memman, binfo->vram, binfo->scrnx, binfo->scrny);
	sht_back  = sheet_alloc(shtctl);
	sht_mouse = sheet_alloc(shtctl);
	buf_back  = (unsigned char *) memman_alloc_4k(memman, binfo->scrnx * binfo->scrny);
	sheet_setbuf(sht_back, buf_back, binfo->scrnx, binfo->scrny, -1); /* �����F�Ȃ� */
	sheet_setbuf(sht_mouse, buf_mouse, 16, 16, 99);
	init_screen8(buf_back, binfo->scrnx, binfo->scrny);
	init_mouse_cursor8(buf_mouse, 99);
	sheet_slide(shtctl, sht_back, 0, 0);
	mx = (binfo->scrnx - 16) / 2; /* ��ʒ����ɂȂ�悤�ɍ��W�v�Z */
	my = (binfo->scrny - 28 - 16) / 2;
	sheet_slide(shtctl, sht_mouse, mx, my);
	sheet_updown(shtctl, sht_back,  0);
	sheet_updown(shtctl, sht_mouse, 1);
	sprintf(s, "(%3d, %3d) %3d", mx, my, serial);
	putfonts8_asc(buf_back, binfo->scrnx, 0, 0, COL8_FFFFFF, s);
	sprintf(s, "memory %dMB   free : %dKB",
			memtotal / (1024 * 1024), memman_total(memman) / 1024);
	putfonts8_asc(buf_back, binfo->scrnx, 0, 32, COL8_FFFFFF, s);
	sheet_refresh(shtctl, sht_back, 0, 0, binfo->scrnx, 48);

	for (;;) {
		io_cli();
		if (fifo8_status(&keyfifo) + fifo8_status(&mousefifo) == 0) {
			io_stihlt();
		} else {
			if (fifo8_status(&keyfifo) != 0) {
				i = fifo8_get(&keyfifo);
				io_sti();
				sprintf(s, "%02X", i);
				boxfill8(buf_back, binfo->scrnx, COL8_008484,  0, 16, 15, 31);
				putfonts8_asc(buf_back, binfo->scrnx, 0, 16, COL8_FFFFFF, s);
				sheet_refresh(shtctl, sht_back, 0, 16, 16, 32);
			} else if (fifo8_status(&mousefifo) != 0) {
				i = fifo8_get(&mousefifo);
				io_sti();
				if (mouse_decode(&mdec, i) != 0) {
					/* �f�[�^��3�o�C�g�������̂ŕ\�� */
					sprintf(s, "[lcr %4d %4d]", mdec.x, mdec.y);
					if ((mdec.btn & 0x01) != 0) {
						s[1] = 'L';
					}
					if ((mdec.btn & 0x02) != 0) {
						s[3] = 'R';
					}
					if ((mdec.btn & 0x04) != 0) {
						s[2] = 'C';
					}
					boxfill8(buf_back, binfo->scrnx, COL8_008484, 32, 16, 32 + 15 * 8 - 1, 31);
					putfonts8_asc(buf_back, binfo->scrnx, 32, 16, COL8_FFFFFF, s);
					sheet_refresh(shtctl, sht_back, 32, 16, 32 + 15 * 8, 32);
					/* �}�E�X�J�[�\���̈ړ� */
					mx += mdec.x;
					my += mdec.y;
					if (mx < 0) {
						mx = 0;
					}
					if (my < 0) {
						my = 0;
					}
					if (mx > binfo->scrnx - 16) {
						mx = binfo->scrnx - 16;
					}
					if (my > binfo->scrny - 16) {
						my = binfo->scrny - 16;
					}
					sprintf(s, "(%3d, %3d)", mx, my);
					boxfill8(buf_back, binfo->scrnx, COL8_008484, 0, 0, 79, 15); /* ���W���� */
					putfonts8_asc(buf_back, binfo->scrnx, 0, 0, COL8_FFFFFF, s); /* ���W���� */
					sheet_refresh(shtctl, sht_back, 0, 0, 80, 16);
					sheet_slide(shtctl, sht_mouse, mx, my);
				}
			}
		}
	}
}