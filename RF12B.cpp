#include "CircularBuffer.h"
#include "RF12B.h"

RF12B::RF12B() {}

void RF12B::begin() {
	_mode = RX;
	_rfa = false;
	_remaining = 0;
	_state = STATE_LENGTH;
	_packet_received = false;
	_r_buf_pos = 0;
	
	delay(100);
	portInit();
	rfInit();

	FIFOReset();	
}

void RF12B::portInit() {
	SPI.begin();
	SPI.setClockDivider(SPI_CLOCK_DIV8);
	SPI.setDataMode(SPI_MODE0);
}

void rxISRFunc() {
	RF12.rxISR();
}

void RF12B::rfInit() {
	pinMode(NIRQ_PIN, INPUT);
	pinMode(nFFS_PIN, INPUT);
  
	writeCmd(0x80E7); //EL,EF,868band,12.0pF
    //if (MODE) {
	writeCmd(0x8299); //er,!ebb,ET,ES,EX,!eb,!ew,DC // receive
    //} else {
    //  writeCmd(0x8239); //!er,!ebb,ET,ES,EX,!eb,!ew,DC
    //}
	writeCmd(0xA640); //frequency select
	writeCmd(0xC647); //4.8kbps
	writeCmd(0x94A0); //VDI,FAST,134kHz,0dBm,-103dBm
	writeCmd(0xC2AC); //AL,!ml,DIG,DQD4
	writeCmd(0xCA81); //FIFO8,SYNC,!ff,DR
	writeCmd(0xCED4); //SYNC=2DD4
	writeCmd(0xC483); //@PWR,NO RSTRIC,!st,!fi,OE,EN
	writeCmd(0x9850); //!mp,90kHz,MAX OUT
	writeCmd(0xCC17); //OB1, COB0, LPX, Iddy, CDDITCBW0
	writeCmd(0xE000); //NOT USED
	writeCmd(0xC800); //NOT USED
	writeCmd(0xC040); //1.66MHz,2.2V
	//attachInterrupt(0, rxISR,FALLING);
	attachInterrupt(1, rxISRFunc,RISING);
}

void RF12B::rfSend(unsigned char data){
	while(digitalRead(NIRQ_PIN) == HIGH);
	writeCmd(0xB800 + data);
}
boolean RF12B::packetAvailable() {
	return _packet_received; 
}
boolean RF12B::rfAvailable() {
	return _r_buf_pos;
}
  
void RF12B::rxISR() {
	if (_mode == RX) {
		
		switch (_state) {
			case STATE_LENGTH:
				if ( (writeCmd(0x0000)&0x8000) ) {
					_remaining = writeCmd(0xB000)&0x00FF;
					_recv_buffer[SIZE_OFFSET] = _remaining;
					_state = READ_DATA;
				}
				_r_buf_pos = 0;
			break;
			case READ_DATA:
				if ( (writeCmd(0x0000)&0x8000) ) {
					_recv_buffer[SIZE_OFFSET+1+_r_buf_pos] = writeCmd(0xB000)&0x00FF;
					_remaining--;
				}
				_r_buf_pos++;
				if (_remaining <= 0) {
					_packet_received = true;
					_state = STATE_LENGTH;
					FIFOReset();
				}
			break;
          
		}
	}
    
}

void RF12B::FIFOReset() {
	writeCmd(0xCA81);
	writeCmd(0xCA83);
}


void RF12B::sendPacket(byte * buf, byte length, byte id, uint16_t seq) {
	changeMode(TX);
	byte crc = 0;
  
	writeCmd(0x0000);
	rfSend(0xAA); // PREAMBLE
	rfSend(0xAA);
	rfSend(0xAA);
	rfSend(0x2D); // SYNC
	rfSend(0xD4);

	rfSend(length+5);
	crc = crc8(crc, length+5);
	
	// Send ID
	rfSend(id);
	crc = crc8(crc, id);
	
	// Sequence number HI byte && LOW byte
	rfSend(seq << 8);
	rfSend(seq & 0xff);
	
	crc = crc8(crc, seq << 8);
	crc = crc8(crc, seq & 0xff);
      
	for(int i=0; i<length; i++) {
		rfSend(buf[i]);
		crc = crc8(crc, buf[i]);
	}
	rfSend(crc);
	Serial.print("CRC: ");
	Serial.println(crc,HEX);
      
	rfSend(0xAA); // DUMMY BYTES
	rfSend(0xAA);
	rfSend(0xAA);
      
      /* Back to receiver mode */
	changeMode(RX);
	delay(10);
	status();
      
}
  
  
RFPacket RF12B::recvPacket() {
	_packet_received = false;
	//byte buf[254];
	//memcpy(buf,recv_buffer,r_buf_pos);
	return RFPacket(_recv_buffer,_r_buf_pos);
}
  
  
unsigned int RF12B::writeCmd(unsigned int cmd) {
	digitalWrite(CS_PIN,LOW);
	uint16_t reply = SPI.transfer(cmd >> 8) << 8;
	reply |= SPI.transfer(cmd);
	digitalWrite(CS_PIN,HIGH);

	return reply;
}

void RF12B::changeMode(int mode) {
	_mode = mode;
	if (_mode == TX) {
		writeCmd(0x8239); //!er,!ebb,ET,ES,EX,!eb,!ew,DC
	} else { /* mode == RX */
		writeCmd(0x8299); //er,!ebb,ET,ES,EX,!eb,!ew,DC
	}
}

unsigned char RF12B::crc8(unsigned char crc, unsigned char data) {
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

unsigned int RF12B::status() {
	return writeCmd(0x0000);
}
RF12B RF12;