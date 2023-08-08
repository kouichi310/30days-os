#include "bootpack.h"

unsigned char *ip_buf;

void init_ip(unsigned char *buf){
    ip_buf = buf;
}

void ip_cupsel(struct IP packet){
    unsigned char version_ihl = packet.version << 4 | packet.ihl;
    unsigned char flag_offset = packet.flags << 5 | packet.flaggment_offset[0];
    memcpy(ip_buf,packet.version + packet.ihl,1);
    memcpy(ip_buf+1,packet.tos,1);
    memcpy(ip_buf+2,packet.len,2);
    memcpy(ip_buf+4,packet.identification,2);
    memcpy(ip_buf+6,(packet.flags >> 5)|(packet.flaggment_offset),2);

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