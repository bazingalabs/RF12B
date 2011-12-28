#include "rf_packet.h"

RFPacket::RFPacket() {
	_crc = 0;
	_valid = false;
	_size = 0;
}

RFPacket::RFPacket(byte buf[],byte size) {
	fromBuffer(buf,size);
}

void RFPacket::fromBuffer(byte buf[],byte size) {
	//byte buf[254];
	//memcpy(buf,b,size);
	Serial.println("Packet from buffer");
	Serial.println(_size);
	_size = size;//buf[SIZE_OFFSET];//buf[SIZE_OFFSET];
	_crc = 0;
	// Sanity check size
	if (_size == -1 || _size <= 0 || _size < buf[SIZE_OFFSET]) {
		Serial.print("Size error: ");
		Serial.println(_size);
		return;
	}
	Serial.print("s:");
	Serial.println(_size);
	
	_crc = crc8(_crc, _size);
	
	_id = buf[ID_OFFSET];
	_crc = crc8(_crc, _id);
	
	_seq = (buf[SEQ_OFFSET] << 8) | buf[SEQ_OFFSET+1];//buf[SEQ_OFFSET];
	_crc = crc8(_crc, _seq >> 8);
	_crc = crc8(_crc, _seq & 0xff);
	
	// Copy data
	for (int i=0; i<DATA_LENGTH; i++) {
		_data[i] = buf[DATA_OFFSET+i];
		_crc = crc8(_crc, _data[i]);
	}
	
	// CRC8 byte
	_rcrc = buf[CRC_OFFSET];//buf[_size];
	Serial.print("s:");
	Serial.println(_size);

}

bool RFPacket::valid() {
	// Check crc
	if (_crc == _rcrc) {
		return true;
	} else {
		return false;
	}
	
}

unsigned char RFPacket::crc8(unsigned char crc, unsigned char data) {
	crc = crc ^ data;
	for (int i = 0; i < 8; i++) {
		if (crc & 0x01) {
			crc = (crc >> 1) ^ 0x8C;
		} else {
			crc >>= 1;
		}
	}
	return crc;
}


void RFPacket::dump() {
	Serial.print("PACKET START: ");
	Serial.print("LENGTH: ");
	Serial.println(_size,DEC);
	Serial.print("ID: ");
	Serial.println(_id,DEC);
	Serial.print("SEQ: ");
	Serial.println(_seq,DEC);
	
	for (int i=0; i<DATA_LENGTH; i++) {
		Serial.print("DATA[");
		Serial.print(i);
		Serial.print("]: ");
		Serial.println((char)_data[i]);
	}
	
	Serial.print("CRC: ");
	Serial.println(_rcrc,HEX);
	Serial.print("CRC(calc): ");
	Serial.println(_crc,HEX);
	
	if (valid()) {
		Serial.println("PACKET VALID");
	} else {
		Serial.println("PACKET INVALID");
	}
	Serial.print("PACKET END: ");
}