//debug func
#include "bootpack.h"

int init_serial(){
	io_out8(SERIAL_PORT+1, 0x00);
	io_out8(SERIAL_PORT+3, 0x80);
	io_out8(SERIAL_PORT+0, 0x03);
	io_out8(SERIAL_PORT+1, 0x00);
	io_out8(SERIAL_PORT+3, 0x04);
	io_out8(SERIAL_PORT+2, 0xc7);
	io_out8(SERIAL_PORT+4, 0x0b);
	io_out8(SERIAL_PORT+4, 0x1e);
	io_out8(SERIAL_PORT+0, 0xae);

	if(io_in8(SERIAL_PORT+0) != 0xae){
		return 1;
	}
	
	io_out8(SERIAL_PORT+4, 0x0f);
	return 0;
}

int is_transmit_empty(){
	return io_in8(SERIAL_PORT+5) & 0x20;
}

void write_serial(char a){
	while(is_transmit_empty()==0);
	io_out8(SERIAL_PORT,a);
}

void debug(char str[]){
    int i;
    for(i=0;str[i] != 0x00;i++){
        if(str[i] == '\n'){
            write_serial('\r');
            while(is_transmit_empty()==0);
        }
        write_serial(str[i]);
        while(is_transmit_empty()==0);
    }
    while(is_transmit_empty()==0);
}

//max:10digits
void debug_int(int num){
    debug("\n");
    int i,count=0;
    char digits[10];

    while(num > 0){
        digits[count] = num %10 + '0';
        num/=10;
        count++;
    }
    for(i=count-1;i>=0;i--){
        write_serial(digits[i]);
    }
    while(is_transmit_empty()==0);
}

void debug_hex(unsigned int value, int num_bytes){
    char str;
    int i;

    for(i=0;i<num_bytes;i++){
        str = (value >> (4*(num_bytes-1-i))) & 0xF;
        str+= (str<10)?'0':('A'-10);
        write_serial(str);
    }
    while(is_transmit_empty()==0);
}

void debug_hex_byte(unsigned char *byte, int bytes_len){
    char str;
    int i;

    debug("\n");
    for(i=0;i<bytes_len;i++){
        debug_hex(byte[i],2);
    }
    while(is_transmit_empty()==0);
}