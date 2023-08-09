#include "bootpack.h"

unsigned char *ip_buf;

void init_ip(unsigned char *buf){
    ip_buf = buf;
}

void ip_send(struct IP packet){
    unsigned char version_ihl = (packet.version << 4) | packet.ihl;
    unsigned char flag_offset = (packet.flags << 5) | packet.flaggment_offset;
    packet.flaggment_offset = flag_offset;
    memcpy(ip_buf,&version_ihl,1);
    memcpy(ip_buf+1,&packet.tos,1);
    memcpy(ip_buf+2,&packet.len,2);
    memcpy(ip_buf+4,&packet.identification,2);
    memcpy(ip_buf+6,&packet.flaggment_offset,2);
    memcpy(ip_buf+8,&packet.ttl,1);
    memcpy(ip_buf+9,&packet.protocol,1);
    memcpy(ip_buf+10,&packet.checksum,2);
    memcpy(ip_buf+12,packet.src,4);
    memcpy(ip_buf+16,packet.dst,4);
    memcpy(ip_buf+20,packet.payload,packet.payload_len);

    unsigned short len = packet.ihl*4+packet.payload_len;
    debug("\nIP PACKET\n");
    debug_hex_byte(ip_buf,len);
    unsigned char dst[MAC_ADDR_LEN] = {0x52,0x55,0x0a,0x00,0x02,0x02};
	unsigned char src[MAC_ADDR_LEN] = {0x52,0x54,0x00,0x12,0x34,0x56};
	unsigned char type[ETHERNET_TYPE_LEN] = {0x08,0x00};

    struct ETHERNET frame;
	
    memcpy(frame.dst,dst,MAC_ADDR_LEN);
	memcpy(frame.src,src,MAC_ADDR_LEN);
	memcpy(frame.type,type,ETHERNET_TYPE_LEN);
	memcpy(frame.payload,ip_buf,packet.payload_len);
	frame.len = len;
    ethernet_send(frame);
}

unsigned short get_checksum(struct IP packet){
    int i;
    unsigned short checksum=0;
    unsigned char version_ihl = (packet.version << 4) | packet.ihl;
    unsigned char flag_offset = (packet.flags << 5) | packet.flaggment_offset;
    packet.flaggment_offset = flag_offset;
    memcpy(ip_buf,&version_ihl,1);
    memcpy(ip_buf+1,&packet.tos,1);
    memcpy(ip_buf+2,&packet.len,2);
    memcpy(ip_buf+4,&packet.identification,2);
    memcpy(ip_buf+6,&packet.flaggment_offset,2);
    memcpy(ip_buf+8,&packet.ttl,1);
    memcpy(ip_buf+9,&packet.protocol,1);
    memcpy(ip_buf+10,&packet.checksum,2);
    memcpy(ip_buf+12,packet.src,4);
    memcpy(ip_buf+16,packet.dst,4);

    unsigned short data=0;
    unsigned char checksum_usb = 0,data_usb=0;
    for(i=0;i<(int)packet.ihl*4;i+=2){
        checksum_usb = checksum >> 15;
        data = (((unsigned short)ip_buf[i]) << 8 ) | ((unsigned short)ip_buf[i+1] << 0);
        data_usb = data >> 15;
        checksum += data;
        debug_hex_byte(&checksum,2);
        if((checksum >> 15) == 0 && (checksum_usb == 1 || data_usb == 1)){
            checksum++;
            debug("CHECKSUM");
        }
    }

    checksum ^= 0xffffffff;
    checksum = ((checksum) << 8) | ((checksum) >> 8);
    debug_hex(&checksum,2);
    return checksum;
}