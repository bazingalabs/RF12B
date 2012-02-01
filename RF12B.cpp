#include "RF12B.h"

RF12B::RF12B() {}

void RF12B::begin(float band) {
	_mode = RX;
	_rfa = false;
	_remaining = 0;
	_state = STATE_LENGTH;
	_packet_received = false;
	_r_buf_pos = 0;
	_id = 0;
	_pan_id = 0xD4;
	_band = band;

	
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
  
	// Set default values for each register
	*((uint16_t*) & _config_reg)          = 0x80E7;	//EL,EF,868band,12.0pF
	*((uint16_t*) & _pwr_mgmt_reg)        = 0x8299; //er,!ebb,ET,ES,EX,!eb,!ew,DC // receive
	*((uint16_t*) & _freq_set_reg)        = 0xA640; //frequency select
	*((uint16_t*) & _data_rate_reg)       = 0xC647; //4.8kbps
	*((uint16_t*) & _recv_ctrl_reg)       = 0x94A0; //VDI,FAST,134kHz,0dBm,-103dBm
	*((uint16_t*) & _data_filter_reg)     = 0xC2AC; //AL,!ml,DIG,DQD4
	*((uint16_t*) & _fifo_reset_reg)      = 0xCA81; //FIFO8,SYNC,!ff,DR
    *((uint16_t*) & _sync_pat_reg)	      = 0xCED4; //SYNC=2DD4
	*((uint16_t*) & _afc_reg)             = 0xC483; //@PWR,NO RSTRIC,!st,!fi,OE,EN
	*((uint16_t*) & _tx_conf_reg)         = 0x9850; //!mp,90kHz,MAX OUT
	*((uint16_t*) & _pll_set_reg)         = 0xCC17; //OB1, COB0, LPX, Iddy, CDDIT CBW0
	*((uint16_t*) & _wkup_tmr_reg)        = 0xE000; //NOT USED
	*((uint16_t*) & _low_dut_reg)         = 0xC800; //NOT USED
	*((uint16_t*) & _low_bat_reg)         = 0xC040; //1.66MHz,2.2V
	
	writeCmd(_config_reg); 
	writeCmd(_pwr_mgmt_reg); 
	writeCmd(_freq_set_reg); 
	writeCmd(_data_rate_reg); 
	writeCmd(_recv_ctrl_reg); 
	writeCmd(_data_filter_reg); 
	writeCmd(_fifo_reset_reg); 
	writeCmd(_sync_pat_reg); 
	writeCmd(_afc_reg); 
	writeCmd(_tx_conf_reg); 
	writeCmd(_pll_set_reg); 
	writeCmd(_wkup_tmr_reg); 
	writeCmd(_low_dut_reg); 
	writeCmd(_low_bat_reg); 

	enableISR();
}

void RF12B::disableISR() {
	detachInterrupt(1);
}

void RF12B::enableISR() {
	attachInterrupt(1, rxISRFunc,RISING);
}

void RF12B::rfSend(unsigned char data){
	while(digitalRead(NIRQ_PIN) == HIGH);
	writeCmd(TX_REG_WRITE + data);
}
bool RF12B::packetAvailable() {
	return _packet_received; 
}
bool RF12B::rfAvailable() {
	return _r_buf_pos;
}

void RF12B::setPanID(byte pan_id) {
	_pan_id = pan_id;
	_sync_pat_reg.sync_pat = _pan_id;

	writeCmd(_sync_pat_reg);
}
  
void RF12B::rxISR() {
	if (_mode == RX) {
		
		switch (_state) {
			case STATE_LENGTH:
				if ( status()->tx_ready ) {
					_remaining = writeCmd(READ_FIFO)&0x00FF;
					_recv_buffer[SIZE_OFFSET] = _remaining;
					_state = READ_DATA;
				}
				_r_buf_pos = 0;
			break;
			case READ_DATA:
				if ( status()->tx_ready ) {
					_recv_buffer[SIZE_OFFSET+1+_r_buf_pos] = writeCmd(READ_FIFO)&0x00FF;
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
	_fifo_reset_reg.enable_fifo_fill = false;
	writeCmd(_fifo_reset_reg);
	_fifo_reset_reg.enable_fifo_fill = true;
	writeCmd(_fifo_reset_reg);
}

// Set datarate
//BR=10000000/29/（R+1)/（1+cs*7)
// Optimal cap values for datarate
// 12 nF	8.2 nF		6.8 nF		3.3 nF		1.5 nF		680 pF		270 pF		150 pF		100 pF
//1.2 kbps	2.4 kbps  	4.8 kbps	9.6 kbps	19.2 kbps	38.4 kbps	57.6 kbps	115.2 kbps	256 kbps

void RF12B::setDatarate(uint16_t baud) {
	_data_rate_reg.data_rate = (10000/29/(1+_data_rate_reg.cs*7)/baud)-1;

	// Set correct bandwidth for datarate RX
	if (baud <= 19200) {
		// BW = 67kHz
		_recv_ctrl_reg.baseband_bw = _recv_ctrl_reg.BBB67;
	} else if (baud > 19200 && baud <= 57600) {
		// BW = 134kHz
		_recv_ctrl_reg.baseband_bw = _recv_ctrl_reg.BBB134;
	} else {
		// BW = 200kHz
		_recv_ctrl_reg.baseband_bw = _recv_ctrl_reg.BBB200;
	}

	// Set correct bandwidth for datarate TX
	if (baud < 86200) {
		_pll_set_reg.pll_bandwidth = 0;
	} else {
		_pll_set_reg.pll_bandwidth = 1;
	}
	
	//_data_rate_reg.cs = 0;
	writeCmd(_data_rate_reg);
	writeCmd(_recv_ctrl_reg);
	writeCmd(_pll_set_reg);
}

void RF12B::setChannel(uint8_t channel) {
	//TODO: Divide 433 - 439 by 30 as channels
	// Calculate center fequenties
    int channelFreq = 0;
    // Channel between 1 and 30
    if (channel >= 1 && channel <= 30) {
        channelFreq = (channel * 0.2) + 0.1;
    }

    setFrequency(_band + channelFreq);

}

void RF12B::send(byte * buf, byte length) {
	changeMode(TX);
  
	status();
	rfSend(0xAA); // PREAMBLE
	rfSend(0xAA);
	rfSend(0xAA);

	rfSend(0x2D); // SYNC
	rfSend(_pan_id);

	rfSend(length+1);
	//Serial.println("SENDING:");
	//Serial.println("LENGTH:");
	//Serial.println(length);
	while(length--) {
		//Serial.println(*buf);
		rfSend(*buf++);
	}

	rfSend(0xAA); // DUMMY BYTES
	rfSend(0xAA);
	rfSend(0xAA);
      
      /* Back to receiver mode */
	changeMode(RX);
	//delay(5);
	status();
}

/*void RF12B::sendPacket(RFPacket *packet) {
	send((byte *)packet, packet->size());
}*/

RFPacket RF12B::recvPacket() {
	_packet_received = false;
	//Serial.println("RECV PACKET:");
	//Serial.println("SIZE:");
	//Serial.println(_r_buf_pos-1);

	RFPacket p = RFPacket(&_recv_buffer[1],_r_buf_pos-1);

	//if (p.getType() == DATA_PACKET) {
	//	sendPacket((byte*)"",0);
	//}

	return p;
}


/**
*	Set frequency
* Boundries  	433MHz: 430.09 - 439.7575
*				868MHz: 860.18 - 879.515
*				915MHz: 900.27 - 929.2725
*	
* @param frequency to set
*/
void RF12B::setFrequency(float ffreq) {
	float freq = ffreq;
	uint8_t band = 0;
	uint16_t f = 0;
	if (freq > 430.09 && freq < 439.7575) {
		freq = freq - 430.0;
		f = freq / 0.0025;
		band = 1;
	} else if (freq > 860.18 && freq < 879.515) {
		freq = freq - 860.0;
		f = freq / 0.0050;
		band = 2;
	}	else if (freq > 900.27 && freq < 929.2725) {
		freq = freq - 900.0;
		f = freq / 0.0075;
		band = 3;
	}
	
	_freq_set_reg.freq = f;
	_config_reg.band = band;
	writeCmd(_freq_set_reg);
	writeCmd(_config_reg);
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
		_pwr_mgmt_reg.enable_rx = false;
		_pwr_mgmt_reg.enable_tx = true;
		writeCmd(_pwr_mgmt_reg);
	} else { /* mode == RX */
		_pwr_mgmt_reg.enable_rx = true;
		_pwr_mgmt_reg.enable_tx = false;
		writeCmd(_pwr_mgmt_reg);
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

struct RF12B::StatusReg * RF12B::status() {
	*((uint16_t*) & _status_reg) = writeCmd(0x0000); 
	return & _status_reg;
}
RF12B RF12;