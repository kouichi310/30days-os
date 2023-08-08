#include "bootpack.h"

void dump_viv_did(unsigned char bus, unsigned char dev, unsigned char func){
    unsigned int conf_data = get_pci_conf_reg(bus,dev,func,PCI_CONF_DID_VID);
	unsigned short vendor_id = conf_data & 0x0000ffff;
	unsigned short device_id = conf_data >> 16;

	debug("VENDOR ID");
	debug_hex(vendor_id,4);
	debug("DEVICE ID");
	debug_hex(device_id,4);
}

void dump_command_status(unsigned char bus, unsigned char dev, unsigned char func){
    unsigned int conf_data = get_pci_conf_reg(bus,dev,func,PCI_CONF_STATUS_COMMAND);

    unsigned short command = conf_data & 0x0000ffff;
    unsigned short status = conf_data >> 16;

    debug("\nCOMMAND\n");
    debug_hex(command, 4);

    if (command & PCI_COM_IO_EN)
        debug("IO_EN ");
    if (command & PCI_COM_MEM_EN)
        debug("MEM_EN ");
    if (command & PCI_COM_BUS_MASTER_EN)
        debug("BUS_MASTER_EN ");
    if (command & PCI_COM_SPECIAL_CYCLE)
        debug("SPECIAL_CYCLE ");
    if (command & PCI_COM_MEMW_INV_EN)
        debug("MEMW_INV_EN ");
    if (command & PCI_COM_VGA_PAL_SNP)
        debug("VGA_PAL_SNP ");
    if (command & PCI_COM_PARITY_ERR_RES)
        debug("PARITY_ERR_RES ");
    if (command & PCI_COM_SERR_EN)
        debug("SERR_EN ");
    if (command & PCI_COM_FAST_BACK2BACK_EN)
        debug("FAST_BACK2BACK_EN ");
    if (command & PCI_COM_INTR_DIS)
        debug("INTR_DIS");
    
    debug("\nSTATUS\n");
    debug_hex(status, 4);

    if (status & PCI_STAT_INTR)
        debug("INTR ");
    if (status & PCI_STAT_MULT_FUNC)
        debug("MULT_FUNC ");
    if (status & PCI_STAT_66MHZ)
        debug("66MHZ ");
    if (status & PCI_STAT_FAST_BACK2BACK)
        debug("FAST_BACK2BACK ");
    if (status & PCI_STAT_DATA_PARITY_ERR)
        debug("DATA_PARITY_ERR ");

    switch (status & PCI_STAT_DEVSEL_MASK) {
        case PCI_STAT_DEVSEL_FAST:
            debug("DEVSEL_FAST ");
            break;
        case PCI_STAT_DEVSEL_MID:
            debug("DEVSEL_MID ");
            break;
        case PCI_STAT_DEVSEL_LOW:
            debug("DEVSEL_LOW ");
            break;
    }

    if (status & PCI_STAT_SND_TARGET_ABORT)
        debug("SND_TARGET_ABORT ");
    if (status & PCI_STAT_RCV_TARGET_ABORT)
        debug("RCV_TARGET_ABORT ");
    if (status & PCI_STAT_RCV_MASTER_ABORT)
        debug("RCV_MASTER_ABORT ");
    if (status & PCI_STAT_SYS_ERR)
        debug("SYS_ERR ");
    if (status & PCI_STAT_PARITY_ERR)   
        debug("PARITY_ERR");
    debug("\n");
}

void dump_bar(unsigned char bus,unsigned char dev, unsigned char func){
    unsigned int bar = get_pci_conf_reg(bus,dev,func,PCI_CONF_BAR);
    debug("\nBAR");
    debug_hex(bar,8);
    debug("\n");

    if(bar & PCI_BAR_MASK_IO){
        debug("IO BASE");
        debug_hex(bar & PCI_BAR_MASK_IO_ADDR,8);
        debug("\n");
    }else{
        unsigned int bar_32;
        unsigned long long bar_upper;
        unsigned long long bar_64;
        switch (bar&PCI_BAR_MASK_MEM_TYPE)
        {
        case PCI_BAR_MEM_TYPE_32BIT:
            debug("MEM BASE 32BIT");
            bar_32 = bar & PCI_BAR_MASK_MEM_ADDR;
            debug_hex(bar_32, 8);
            
            debug("\n");
            break;
        
        case PCI_BAR_MEM_TYPE_1M:
            debug("MEM BASE 1M");
            bar_32 = bar & PCI_BAR_MASK_MEM_ADDR;
            debug_hex(bar_32,8);
            break;
        case PCI_BAR_MEM_TYPE_64BIT:
            bar_upper = get_pci_conf_reg(bus,dev,func,PCI_CONF_BAR+4);
            bar_64 = (bar_upper << 32) + (bar & PCI_BAR_MASK_MEM_ADDR);
            debug("MEM BASE 64BIT");
            debug_hex(bar_64,16);
            debug("\n");
        }
        if (bar & PCI_BAR_MASK_MEM_PREFETCHABLE)
        {
            debug("PREFETCHABLE\n");
        }else{
            debug("NON PREFETCHABLE\n");
        }
        
    }
}

unsigned int get_pci_conf_reg(unsigned char bus, unsigned char dev, unsigned char func, unsigned char reg){
    union PCI_CONFIG_ADDRESS_REGISTER conf_addr;

	conf_addr.raw = 0;
	conf_addr.bus_num = bus;
	conf_addr.dev_num = dev;
	conf_addr.func_num = func;
	conf_addr.reg_addr = reg;
	conf_addr.enable_bit = 1;
	io_out32(CONFIG_ADDRESS, conf_addr.raw);

    return io_in32(CONFIG_DATA);
}

void set_pci_conf_reg(unsigned char bus, unsigned char dev, unsigned char func, unsigned char reg, unsigned int val){
    union PCI_CONFIG_ADDRESS_REGISTER conf_addr;

	conf_addr.raw = 0;
	conf_addr.bus_num = bus;
	conf_addr.dev_num = dev;
	conf_addr.func_num = func;
	conf_addr.reg_addr = reg;
	conf_addr.enable_bit = 1;
	io_out32(CONFIG_ADDRESS, conf_addr.raw);

    io_out32(CONFIG_DATA, val);
}
