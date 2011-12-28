#ifndef RF12B_H
#define RF12B_H


#include "CircularBuffer.h"
#include "stdint.h"
#include <SPI.h>
#include "Arduino.h"
#include "rf12b_def.h"
#include "rf_packet.h"


class RF12B {
	public:
		void portInit();
		void rfInit() ;
		void rfSend(unsigned char data);
		boolean packetAvailable();
		boolean rfAvailable();
	    void rxISR();
		void FIFOReset();
		void sendPacket(byte * buf, byte length, byte id=0, uint16_t seq=0);
		void sendPacket(RFPacket){};
		RFPacket recvPacket();
		unsigned int writeCmd(unsigned int cmd);
		void changeMode(int _mode);
		unsigned char crc8(unsigned char crc, unsigned char data);
		unsigned int status();
		static void callbackISR();
		void begin();
		RF12B();
	private:
		int _mode;
		boolean _rfa;
		byte _remaining;
		byte _state;
		boolean _packet_received;
		byte _recv_buffer[BUFFER_SIZE];
		byte _r_buf_pos;
};
void rxISRFunc();
extern RF12B RF12;
#endif