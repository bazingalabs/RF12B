#include "rf_packet.h"

RFPacket::RFPacket(byte buf[],byte size) {
	_rcrc = 0;
	parse(buf,size);
}

RFPacket::RFPacket(byte * buf, byte length, byte id, uint16_t seq, byte type) {
	byte crc = 0;
	_rcrc = 0;
  	rfpacket.p.crc = 0;
	rfpacket.p.size = length + PACKET_HEADER_SIZE;
	//crc = crc8(crc, rfpacket.p.size);
	
	// Send ID
	rfpacket.p.id = id;
	//crc = crc8(crc, rfpacket.p.id);
	
	// Sequence number HI byte && LOW byte
	rfpacket.p.seq = seq;
	//crc = crc8(crc, seq << 8);
	//crc = crc8(crc, seq & 0xff);

	// Send type
	rfpacket.p.type = type;
	//crc = crc8(crc, rfpacket.p.type);
      
	for(int i=0; i<length; i++) {
		rfpacket.p.data[i] = buf[i];
		//crc = crc8(crc, buf[i]);
	}

	for (int i = 0; i < rfpacket.p.size; ++i) {
		crc = crc8(crc,rfpacket.buf[i]);
	}
	rfpacket.p.crc = crc;    
}

void RFPacket::parse(byte * buf,byte size) {
	uint8_t crc = 0;
	for (int i = 0; i < size; ++i)
	{
		rfpacket.buf[i] = buf[i];
		if (&rfpacket.buf[i] == (uint8_t*)&rfpacket.p.crc) {
			crc = crc8(crc,0);
		} else {
			crc = crc8(crc,buf[i]);
		}
	}
	_rcrc = crc;
	/*_size = size;
	_crc = 0;
	// Clear buffer
	memset(_data, '\0', sizeof(_data));
	// Sanity check size
	if (_size == -1 || _size <= 0 || _size < buf[SIZE_OFFSET]) {
		Serial.print("Size error: ");
		Serial.println(_size);
		return;
	}
	_crc = crc8(_crc, _size);
	
	_id = buf[ID_OFFSET];
	_crc = crc8(_crc, _id);
	
	_seq = (buf[SEQ_OFFSET] << 8) | buf[SEQ_OFFSET+1];//buf[SEQ_OFFSET];
	_crc = crc8(_crc, _seq >> 8);
	_crc = crc8(_crc, _seq & 0xff);

	_type = buf[TYPE_OFFSET];
	_crc = crc8(_crc, _type);
	
	// Copy data
	for (int i=0; i<DATA_LENGTH; i++) {
		_data[i] = buf[DATA_OFFSET+i];
		_crc = crc8(_crc, _data[i]);
	}
	
	// CRC8 byte
	_rcrc = buf[CRC_OFFSET];*/
}

uint8_t RFPacket::size() {
	return rfpacket.p.size;
}

bool RFPacket::valid() {
	// Check crc
	if (rfpacket.p.crc == _rcrc) {
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

byte RFPacket::dataSize() {
	return rfpacket.p.size-PACKET_HEADER_SIZE;
}

byte RFPacket::getID() {
	return rfpacket.p.id;
}

byte RFPacket::getType() {
	return rfpacket.p.type;
}

uint16_t RFPacket::getSeq() {
	return rfpacket.p.seq;
}

void RFPacket::getData(byte buf[], byte size) {
	memcpy(buf, rfpacket.p.data,size);
}

void RFPacket::dump() {
	Serial.println("PACKET START: ");
	Serial.print("   LENGTH: ");
	Serial.println(rfpacket.p.size,DEC);
	Serial.print("   ID: ");
	Serial.println(rfpacket.p.id,DEC);
	Serial.print("   SEQ: ");
	Serial.println(rfpacket.p.seq,DEC);
	Serial.print("   DataSize: ");
	Serial.println(dataSize(),DEC);
	/*Serial.print("   TYPE: ");
	if (_type == DATA_PACKET) {
		Serial.println("data");
	} else if (_type == ACK_PACKET) {
		Serial.println("ack");
	}
	*/
	
	for (int i=0; i<dataSize(); i++) {
		Serial.print("   DATA[");
		Serial.print(i);
		Serial.print("]: ");
		Serial.println((char)rfpacket.p.data[i]);
	}
	
	Serial.print("   CRC: ");
	Serial.println(rfpacket.p.crc,HEX);
	Serial.print("   CRC(calc): ");
	Serial.println(_rcrc,HEX);
	/*
	if (valid()) {
		Serial.println("   PACKET VALID");
	} else {
		Serial.println("   PACKET INVALID");
	}*/
	Serial.println("PACKET END: ");
}