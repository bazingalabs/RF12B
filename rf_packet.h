#ifndef RF_PACKET_H
#define RF_PACKET_H

#include <stdint.h>
#include "Arduino.h"
#include <Printable.h>
#include <stdio.h>

#define BUFFER_SIZE 254
#define SIZE_OFFSET 0
#define ID_OFFSET 1
#define SEQ_OFFSET 2
#define TYPE_OFFSET 4
#define DATA_OFFSET 5
#define CRC_OFFSET _size - 1
#define DATA_LENGTH _size - 6
#define PACKET_HEADER 6

#define DATA_PACKET 	1
#define ACK_PACKET  	2

class RFPacket {
	public:
		RFPacket(){
			_crc = 0;
			_valid = false;
			_size = 0;
			_type = DATA_PACKET;
			
		};
		RFPacket(byte buf[],byte size);
		void fromBuffer(byte buf[], byte size);
		uint16_t getSize();
		//uint16_t getID();
		uint16_t getSeqNr();
		uint16_t getCRC();
		uint8_t getType();
		bool valid();
		unsigned char crc8(unsigned char crc, unsigned char data);
		void dump();
		byte dataSize();
	    inline operator const char *() {
			return (char *)_data; 
		}
	    // compare to another string
	    bool operator==(const char *str) 
	    { return _size > 0 && !strcmp((char *)_data, str); }
		byte getID();
		uint16_t getSeq();
		void getData(byte buf[], byte size);
		
	private:
		uint8_t _size;
		uint16_t _id;
		uint16_t _seq;
		uint8_t _data[BUFFER_SIZE];
		uint16_t _crc;
		uint16_t _rcrc;
		uint8_t _type;
		bool _valid;
};

#endif