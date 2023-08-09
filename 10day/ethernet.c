#include "bootpack.h"

unsigned char *ethernet_buf;

void init_ethernet(unsigned char *buf){
    ethernet_buf = buf;
}

struct ETHERNET ethernet_receive(void){
    unsigned short i,payload_len,len;
    struct ETHERNET frame;

    len = receive_frame(ethernet_buf);

    if(len>0){
        debug_hex_byte(ethernet_buf,len);
        payload_len = len-2*MAC_ADDR_LEN-ETHERNET_TYPE_LEN;
        memcpy(frame.dst,ethernet_buf,MAC_ADDR_LEN);
        memcpy(frame.src,ethernet_buf+MAC_ADDR_LEN,MAC_ADDR_LEN);
        memcpy(frame.type,ethernet_buf+2*MAC_ADDR_LEN,ETHERNET_TYPE_LEN);
        memcpy(frame.payload,ethernet_buf+2*MAC_ADDR_LEN+ETHERNET_TYPE_LEN,payload_len);
        debug("\nRECEIVE FRAME\nDST MAC ADDR : ");
        debug_hex_byte(frame.dst,MAC_ADDR_LEN);
        debug("\nSRC MAC ADDR : ");
        debug_hex_byte(frame.src,MAC_ADDR_LEN);
        debug("\nTYPE : ");
        debug_hex_byte(frame.type,ETHERNET_TYPE_LEN);
        debug("\nPAYLOAD : ");
        debug_hex_byte(frame.payload,payload_len);
        debug("\n");
    }
    return frame;
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

    return status;
}