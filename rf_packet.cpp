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
    read_ptr = rfpacket.p.data;  
}


RFPacket::RFPacket(CircularBuffer<byte,200> *  buf, byte size, byte id, uint16_t seq, byte type) {
	byte crc = 0;
	_rcrc = 0;
  	rfpacket.p.crc = 0;
	rfpacket.p.size = size + PACKET_HEADER_SIZE;
	//Serial.println("Size");
	//Serial.println(size);
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
      
	for(int i=0; i<size; i++) {
		rfpacket.p.data[i] = buf->pop();
		//crc = crc8(crc, buf[i]);
	}

	for (int i = 0; i < rfpacket.p.size; ++i) {
		crc = crc8(crc,rfpacket.buf[i]);
	}
	rfpacket.p.crc = crc;   
    
    read_ptr = rfpacket.p.data;   
}


//RFPacket::RFPacket(CircularBuffer<byte, uint16_t> *  buf, byte id, uint16_t seq, byte type);

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
    
    read_ptr = rfpacket.p.data;
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

int16_t RFPacket::readInt16() {
    int16_t ret;
    ret = (*(int16_t *) read_ptr);
    read_ptr += sizeof(int16_t);
    return ret;
}
uint16_t RFPacket::readUint16() {
    uint16_t ret;
    ret = (*(uint16_t *) read_ptr);
    read_ptr += sizeof(uint16_t);
    return ret;
}
int8_t RFPacket::readInt8(){
    int8_t ret;
    ret = (*(int8_t *) read_ptr);
    read_ptr += sizeof(int8_t);
    return ret;
}
uint8_t RFPacket::readUint8(){
    uint8_t ret;
    ret = (*(uint8_t *) read_ptr);
    read_ptr += sizeof(uint8_t);
    return ret;
}
char RFPacket::readChar(){
    int8_t ret;
    ret = (*(int8_t *) read_ptr);
    read_ptr += sizeof(int8_t);
    return ret;
}
void RFPacket::readString(byte buf[], byte size) {
	memcpy(buf, read_ptr,size);
    read_ptr += size;
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
