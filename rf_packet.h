#ifndef RF_PACKET_H
#define RF_PACKET_H

#include <stdint.h>
#include "CircularBuffer.h"
#include "Arduino.h"
#include <Printable.h>
#include <stdio.h>

#define BUFFER_SIZE 254
#define SIZE_OFFSET 0
#define ID_OFFSET 1
#define SEQ_OFFSET 2
#define DATA_OFFSET 4
#define CRC_OFFSET _size - 1
#define DATA_LENGTH _size - 5

class RFPacket {
	public:
		RFPacket(){
			_crc = 0;
			_valid = false;
			_size = 0;
			
		};
		RFPacket(byte buf[],byte size);
		void fromBuffer(byte buf[], byte size);
		uint16_t getSize();
		//uint16_t getID();
		uint16_t getSeqNr();
		uint16_t getCRC();
		bool valid();
		unsigned char crc8(unsigned char crc, unsigned char data);
		void dump();
		byte dataSize();
	    inline operator const char *() {
			char tmp[BUFFER_SIZE];
			memcpy(tmp,_data,DATA_LENGTH);
			tmp[DATA_LENGTH] = '\0'; 
			return tmp; 
		}
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
		bool _valid;
};

#endif