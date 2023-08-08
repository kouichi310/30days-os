/* asmhead.nas */
struct BOOTINFO { /* 0x0ff0-0x0fff */
	char cyls; /* �u�[�g�Z�N�^�͂ǂ��܂Ńf�B�X�N��ǂ񂾂̂� */
	char leds; /* �u�[�g���̃L�[�{�[�h��LED�̏�� */
	char vmode; /* �r�f�I���[�h  ���r�b�g�J���[�� */
	char reserve;
	short scrnx, scrny; /* ��ʉ𑜓x */
	char *vram;
};
#define ADR_BOOTINFO	0x00000ff0

/* naskfunc.nas */
void io_hlt(void);
void io_cli(void);
void io_sti(void);
void io_stihlt(void);
int io_in8(int port);
int io_in32(int port);
void io_out8(int port, int data);
void io_out32(int port, int data);
int io_load_eflags(void);
void io_store_eflags(int eflags);
void load_gdtr(int limit, int addr);
void load_idtr(int limit, int addr);
int load_cr0(void);
void store_cr0(int cr0);
void asm_inthandler21(void);
void asm_inthandler27(void);
void asm_inthandler2c(void);
unsigned int memtest_sub(unsigned int start, unsigned int end);

/* fifo.c */
struct FIFO8 {
	unsigned char *buf;
	int p, q, size, free, flags;
};
void fifo8_init(struct FIFO8 *fifo, int size, unsigned char *buf);
int fifo8_put(struct FIFO8 *fifo, unsigned char data);
int fifo8_get(struct FIFO8 *fifo);
int fifo8_status(struct FIFO8 *fifo);

/* graphic.c */
void init_palette(void);
void set_palette(int start, int end, unsigned char *rgb);
void boxfill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1);
void init_screen8(char *vram, int x, int y);
void putfont8(char *vram, int xsize, int x, int y, char c, char *font);
void putfonts8_asc(char *vram, int xsize, int x, int y, char c, unsigned char *s);
void init_mouse_cursor8(char *mouse, char bc);
void putblock8_8(char *vram, int vxsize, int pxsize,
	int pysize, int px0, int py0, char *buf, int bxsize);
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

/* dsctbl.c */
struct SEGMENT_DESCRIPTOR {
	short limit_low, base_low;
	char base_mid, access_right;
	char limit_high, base_high;
};
struct GATE_DESCRIPTOR {
	short offset_low, selector;
	char dw_count, access_right;
	short offset_high;
};
void init_gdtidt(void);
void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar);
void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar);
#define ADR_IDT			0x0026f800
#define LIMIT_IDT		0x000007ff
#define ADR_GDT			0x00270000
#define LIMIT_GDT		0x0000ffff
#define ADR_BOTPAK		0x00280000
#define LIMIT_BOTPAK	0x0007ffff
#define AR_DATA32_RW	0x4092
#define AR_CODE32_ER	0x409a
#define AR_INTGATE32	0x008e

/* int.c */
void init_pic(void);
void inthandler27(int *esp);
#define PIC0_ICW1		0x0020
#define PIC0_OCW2		0x0020
#define PIC0_IMR		0x0021
#define PIC0_ICW2		0x0021
#define PIC0_ICW3		0x0021
#define PIC0_ICW4		0x0021
#define PIC1_ICW1		0x00a0
#define PIC1_OCW2		0x00a0
#define PIC1_IMR		0x00a1
#define PIC1_ICW2		0x00a1
#define PIC1_ICW3		0x00a1
#define PIC1_ICW4		0x00a1

/* keyboard.c */
void inthandler21(int *esp);
void wait_KBC_sendready(void);
void init_keyboard(void);
extern struct FIFO8 keyfifo;
#define PORT_KEYDAT		0x0060
#define PORT_KEYCMD		0x0064

/* mouse.c */
struct MOUSE_DEC {
	unsigned char buf[3], phase;
	int x, y, btn;
};
void inthandler2c(int *esp);
void enable_mouse(struct MOUSE_DEC *mdec);
int mouse_decode(struct MOUSE_DEC *mdec, unsigned char dat);
extern struct FIFO8 mousefifo;

/* memory.c */
#define MEMMAN_FREES		4090	/* ����Ŗ�32KB */
#define MEMMAN_ADDR			0x003c0000
struct FREEINFO {	/* ������� */
	unsigned int addr, size;
};
struct MEMMAN {		/* �������Ǘ� */
	int frees, maxfrees, lostsize, losts;
	struct FREEINFO free[MEMMAN_FREES];
};
unsigned int memtest(unsigned int start, unsigned int end);
void memman_init(struct MEMMAN *man);
unsigned int memman_total(struct MEMMAN *man);
unsigned int memman_alloc(struct MEMMAN *man, unsigned int size);
int memman_free(struct MEMMAN *man, unsigned int addr, unsigned int size);
unsigned int memman_alloc_4k(struct MEMMAN *man, unsigned int size);
int memman_free_4k(struct MEMMAN *man, unsigned int addr, unsigned int size);

/* sheet.c */
#define MAX_SHEETS		256
struct SHEET {
	unsigned char *buf;
	int bxsize, bysize, vx0, vy0, col_inv, height, flags;
};
struct SHTCTL {
	unsigned char *vram;
	int xsize, ysize, top;
	struct SHEET *sheets[MAX_SHEETS];
	struct SHEET sheets0[MAX_SHEETS];
};
struct SHTCTL *shtctl_init(struct MEMMAN *memman, unsigned char *vram, int xsize, int ysize);
struct SHEET *sheet_alloc(struct SHTCTL *ctl);
void sheet_setbuf(struct SHEET *sht, unsigned char *buf, int xsize, int ysize, int col_inv);
void sheet_updown(struct SHTCTL *ctl, struct SHEET *sht, int height);
void sheet_refresh(struct SHTCTL *ctl, struct SHEET *sht, int bx0, int by0, int bx1, int by1);
void sheet_slide(struct SHTCTL *ctl, struct SHEET *sht, int vx0, int vy0);
void sheet_free(struct SHTCTL *ctl, struct SHEET *sht);

//debug.c
#define SERIAL_PORT 0x3f8
void debug(char str[]);
void debug_int(int num);
void debug_hex(unsigned int num, int num_bytes);
void write_serial(char a);
int is_transmit_empty();
int init_serial();

//pci.c
#define PCI_CONF_DID_VID		0x00
#define PCI_CONF_STATUS_COMMAND 0x04
#define PCI_CONF_BAR			0x10

#define CONFIG_ADDRESS 			0x0cf8
#define CONFIG_DATA				0x0cfc

#define PCI_COM_IO_EN 			(1U << 0)
#define PCI_COM_MEM_EN 			(1U << 1)
#define PCI_COM_BUS_MASTER_EN 	(1U << 2)
#define PCI_COM_SPECIAL_CYCLE	(1U << 3)
#define PCI_COM_MEMW_INV_EN			(1U << 4)
#define PCI_COM_VGA_PAL_SNP		(1U << 5)
#define PCI_COM_PARITY_ERR_RES	(1U << 6)
#define PCI_COM_SERR_EN			(1U << 8)
#define PCI_COM_FAST_BACK2BACK_EN	(1U << 9)
#define PCI_COM_INTR_DIS		(1U << 10)

#define PCI_STAT_INTR			(1U << 3)
#define PCI_STAT_MULT_FUNC		(1U << 4)
#define PCI_STAT_66MHZ			(1U << 5)
#define PCI_STAT_FAST_BACK2BACK	(1U << 7)
#define PCI_STAT_DATA_PARITY_ERR (1U << 8)
#define PCI_STAT_DEVSEL_MASK	(3U << 9)
#define PCI_STAT_DEVSEL_FAST	(0x00 << 9)
#define PCI_STAT_DEVSEL_MID 	(0x01 << 9)
#define PCI_STAT_DEVSEL_LOW		(0x10 << 9)
#define PCI_STAT_SND_TARGET_ABORT (1U << 11)
#define PCI_STAT_RCV_TARGET_ABORT (1U << 12)
#define PCI_STAT_RCV_MASTER_ABORT (1U << 13)
#define PCI_STAT_SYS_ERR		(1U << 14)
#define PCI_STAT_PARITY_ERR		(1U << 15)

#define PCI_BAR_MASK_IO			0x00000001
#define PCI_BAR_MASK_MEM_TYPE	0x00000006
#define PCI_BAR_MEM_TYPE_32BIT	0x00000000
#define PCI_BAR_MEM_TYPE_1M		0x00000002
#define PCI_BAR_MEM_TYPE_64BIT	0x00000004
#define PCI_BAR_MASK_MEM_PREFETCHABLE	0x00000008
#define PCI_BAR_MASK_MEM_ADDR	0xfffffff0
#define PCI_BAR_MASK_IO_ADDR	0xfffffffc

union PCI_CONFIG_ADDRESS_REGISTER{
	unsigned int raw;
	struct PCI_CONFIG_ADDRESS_BIT {
		unsigned int reg_addr:8;
		unsigned int func_num:3;
		unsigned int dev_num:5;
		unsigned int bus_num:8;
		unsigned int _reserved:7;
		unsigned int enable_bit:1;
	};
};

#define NIC_BUS_NUM 			0x00
#define NIC_DEV_NUM				0x03
#define NIC_FUNC_NUM			0x0
#define NIC_REG_IMS 			0x00d0
#define NIC_REG_IMC				0x00d8

void dump_viv_did(unsigned char bus, unsigned char dev, unsigned char func);
void dump_command_status(unsigned char bus, unsigned char dev, unsigned char func);
void dump_bar(unsigned char bus,unsigned char dev, unsigned char func);
void dump_nic_ims(void);

unsigned int get_pci_conf_reg(unsigned char bus, unsigned char dev, unsigned char func, unsigned char reg);
void set_pci_conf_reg(unsigned char bus, unsigned char dev, unsigned char func, unsigned char reg, unsigned int val);

void init_nic(void);
unsigned int get_nic_reg_base(void);
unsigned int get_nic_reg(unsigned short reg);
void set_nic_reg(unsigned short reg, unsigned int val);
static void disable_nic_interrupt(void);