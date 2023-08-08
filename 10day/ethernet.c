#include "bootpack.h"

unsigned char *ethernet_buf;

void init_ethernet(unsigned char *buf){
    ethernet_buf = buf;
}

unsigned short ethernet_send(struct ETHERNET frame){
    memcpy(ethernet_buf,frame.dst,MAC_ADDR_LEN);
    memcpy(ethernet_buf+MAC_ADDR_LEN,frame.src,MAC_ADDR_LEN);
    memcpy(ethernet_buf+2*MAC_ADDR_LEN,frame.type,ETHERNET_TYPE_LEN);
    memcpy(ethernet_buf+2*MAC_ADDR_LEN+ETHERNET_TYPE_LEN,frame.payload,frame.len);

    unsigned short len = 2*MAC_ADDR_LEN+ETHERNET_TYPE_LEN+frame.len;
    debug_hex_byte(ethernet_buf,len);
    debug("\n");
    unsigned char status = send_frame(ethernet_buf, len);
    switch (status)
    {
        case NIC_TDESC_STA_DD:
            debug("\nEF\n");
            break;
        case NIC_TDESC_STA_EC:
            debug("\nEC\n");
            break;
        case NIC_TDESC_STA_LC:
            debug("\nLC\n");
            break;
        case NIC_TDESC_STA_TU:
            debug("\nTU\n");
            break;
        default:
            debug("ERROR");
            debug_hex(status,1);
            break;
    }
}