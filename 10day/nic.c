#include "bootpack.h"

struct RXDESC {
	unsigned long long buffer_address;
	unsigned short length;
	unsigned short packet_checksum;
	unsigned char status;
	unsigned char errors;
	unsigned short special;
};

struct TXDESC {
	unsigned long long buffer_address;
	unsigned short length;
	unsigned char cso;
	unsigned char cmd;
	unsigned char sta:4;
	unsigned char _rsv:4;
	unsigned char css;
	unsigned short special;
};

static unsigned int nic_reg_base;
static unsigned char rx_buffer[RXDESC_NUM][PACKET_BUFFER_SIZE];
static unsigned char rxdesc_data[(sizeof(struct RXDESC) * RXDESC_NUM) + ALIGN_MARGIN];
static struct RXDESC *rxdesc_base;
static unsigned short current_rx_index;
static unsigned char txdesc_data[(sizeof(struct TXDESC) * TXDESC_NUM)+ ALIGN_MARGIN];
static struct TXDESC *txdesc_base;
static unsigned short current_tx_index;
static unsigned char nic_mac_addr[6]={0};

unsigned int get_nic_reg_base(void){
    unsigned bar = get_pci_conf_reg(NIC_BUS_NUM,NIC_DEV_NUM,NIC_FUNC_NUM,PCI_CONF_BAR);
    return bar & PCI_BAR_MASK_MEM_ADDR;
}

unsigned int get_nic_reg(unsigned short reg){
    unsigned long long addr = nic_reg_base + reg;
    return *(volatile unsigned int *)addr;
}

void set_nic_reg(unsigned short reg, unsigned int val){
    unsigned long long addr = nic_reg_base + reg;
    *(volatile unsigned int *)addr = val;
}

void dump_nic_ims(void){
    unsigned int ims = get_nic_reg(NIC_REG_IMS);
    debug("IMS");
    debug_hex(ims,8);
    debug("\n");
}

unsigned short receive_frame(void *buf){
    unsigned short len = 0;
    struct RXDESC *current_rxdesc = rxdesc_base + current_rx_index;
    //debug_hex(current_rxdesc->status,1);
    if(current_rxdesc->status & NIC_RDESC_STAT_DD){
        len = current_rxdesc->length;
        memcpy(buf, (void *)current_rxdesc->buffer_address,current_rxdesc->length);
        current_rxdesc->status = 0;
        set_nic_reg(NIC_REG_RDT, current_rx_index);
        current_rx_index = (current_rx_index+1)% RXDESC_NUM;
    }
    return len;
}

static void get_mac_addr_rar(void){
    unsigned int ral_0 = get_nic_reg(NIC_REG_RAL(0));
    unsigned int rah_0 = get_nic_reg(NIC_REG_RAH(0));

    nic_mac_addr[0] = ral_0&0x000000ff;
    nic_mac_addr[1] = (ral_0 >> 8)&0x000000ff;
    nic_mac_addr[2] = (ral_0 >> 16)&0x000000ff;
    nic_mac_addr[3] = (ral_0 >> 24)&0x000000ff;
    nic_mac_addr[4] = rah_0&0x000000ff;
    nic_mac_addr[5] = (rah_0 >> 8)&0x000000ff;
}

static void get_mac_addr(void){
    get_mac_addr_rar();

    unsigned char i;
    for(i=0;i<MAC_ADDR_LEN;i++){
        debug_hex(nic_mac_addr[i],2);
        if(i!=5)debug(":");
    }
    debug("\n");
}

unsigned char send_frame(void *buf, unsigned short len){
    struct TXDESC *current_txdesc = txdesc_base + current_tx_index;
    current_txdesc->buffer_address = (unsigned long long)buf;
    current_txdesc->length = len;
    current_txdesc->sta = 0;

    current_tx_index = (current_tx_index + 1) % TXDESC_NUM;
    set_nic_reg(NIC_REG_TDT, current_tx_index);

    unsigned char send_status = 0;
    while (!send_status)
    {
        send_status = current_txdesc->sta & 0x0f;
    }

    return send_status;
}

unsigned short dump_frame(void){
    unsigned char buf[PACKET_BUFFER_SIZE];
    unsigned short len;
    len = receive_frame(buf);

    unsigned short i;
    for(i=0;i<len;i++){
        debug_hex(buf[i],2);
        debug(" ");
        if(((i+1)%24)==0)debug("\n");
    }
    if(len > 0){
        debug("\n");
    }
    return len;
}

static void disable_nic_interrupt(void){
    unsigned int conf_data = get_pci_conf_reg(NIC_BUS_NUM,NIC_DEV_NUM,NIC_FUNC_NUM,PCI_CONF_STATUS_COMMAND);
    conf_data &= 0x0000ffff;
    conf_data != PCI_COM_INTR_DIS;
    set_pci_conf_reg(NIC_BUS_NUM,NIC_DEV_NUM,NIC_FUNC_NUM,PCI_CONF_STATUS_COMMAND, conf_data);
    set_nic_reg(NIC_REG_IMC,0xffffffff);
}

static void rx_init(void){
    unsigned int i;
    unsigned long long rxdesc_addr = (unsigned long long )rxdesc_data;
    
    rxdesc_addr = (rxdesc_addr + ALIGN_MARGIN) & 0xfffffffffffffff0;
    rxdesc_base = (struct RXDESC *)rxdesc_addr;

    struct RXDESC *current_rxdesc = rxdesc_base;
    for(i=0;i<RXDESC_NUM;i++){
        current_rxdesc->buffer_address = (unsigned long long)rx_buffer[i];
        current_rxdesc->status = 0;
        current_rxdesc->errors = 0;
        current_rxdesc++;
    }

    set_nic_reg(NIC_REG_RDBAH,rxdesc_addr >> 32);
    set_nic_reg(NIC_REG_RDBAL,rxdesc_addr& 0x00000000ffffffff);
    set_nic_reg(NIC_REG_RDLEN,sizeof(struct RXDESC)*RXDESC_NUM);

    current_rx_index = 0;
    set_nic_reg(NIC_REG_RDH, current_rx_index);
    set_nic_reg(NIC_REG_RDT, RXDESC_NUM-1);

    set_nic_reg(NIC_REG_RCTL, PACKET_RBSIZE_BIT|NIC_RCTL_BAM|NIC_RCTL_MPE|NIC_RCTL_UPE|NIC_RCTL_SBP|NIC_RCTL_EN|(1U << 26));
    unsigned int setting = get_nic_reg(NIC_REG_RCTL);
}

static void tx_init(void){
    unsigned int i;
    unsigned long long txdesc_addr = (unsigned long long)txdesc_data;
    txdesc_addr = (txdesc_addr + ALIGN_MARGIN) & 0xfffffffffffffff0;

	/* txdescの初期化 */
	txdesc_base = (struct TXDESC *)txdesc_addr;
	struct TXDESC *cur_txdesc = txdesc_base;
	for (i = 0; i < TXDESC_NUM; i++) {
		cur_txdesc->buffer_address = 0;
		cur_txdesc->length = 0;
		cur_txdesc->cso = 0;
		cur_txdesc->cmd = NIC_TDESC_CMD_RS | NIC_TDESC_CMD_EOP;
		cur_txdesc->sta = 0;
		cur_txdesc->_rsv = 0;
		cur_txdesc->css = 0;
		cur_txdesc->special = 0;
		cur_txdesc++;
	}

	/* txdescの先頭アドレスとサイズをNICレジスタへ設定 */
	set_nic_reg(NIC_REG_TDBAH, txdesc_addr >> 32);
	set_nic_reg(NIC_REG_TDBAL, txdesc_addr & 0x00000000ffffffff);
	set_nic_reg(NIC_REG_TDLEN, sizeof(struct TXDESC) * TXDESC_NUM);

	/* 先頭と末尾のインデックスをNICレジスタへ設定 */
	current_tx_index = 0;
	set_nic_reg(NIC_REG_TDH, current_tx_index);
	set_nic_reg(NIC_REG_TDT, current_tx_index);

	/* NICの送信動作設定 */
	set_nic_reg(NIC_REG_TCTL, (0x40 << NIC_TCTL_COLD_SHIFT)
		    | (0x0f << NIC_TCTL_CT_SHIFT) | NIC_TCTL_PSP | NIC_TCTL_EN);
}

void init_nic(void){
    unsigned int conf_data = get_pci_conf_reg(NIC_BUS_NUM,NIC_DEV_NUM,NIC_FUNC_NUM,PCI_CONF_STATUS_COMMAND);
	conf_data &= 0x0000ffff;
	conf_data |= PCI_COM_INTR_DIS;
    conf_data |= PCI_COM_BUS_MASTER_EN;

	set_pci_conf_reg(NIC_BUS_NUM,NIC_DEV_NUM,NIC_FUNC_NUM,PCI_CONF_STATUS_COMMAND,conf_data);
	nic_reg_base = get_nic_reg_base();
    
    disable_nic_interrupt();
    dump_command_status(NIC_BUS_NUM,NIC_DEV_NUM,NIC_FUNC_NUM);
    //dump_bar(NIC_BUS_NUM,NIC_DEV_NUM,NIC_FUNC_NUM);
    //dump_nic_ims();

    rx_init();
    tx_init();
    get_mac_addr();
}