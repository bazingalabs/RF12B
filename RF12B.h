#ifndef RF12B_H
#define RF12B_H

#include "stdint.h"
#include <SPI.h>
#include "Arduino.h"
#include "rf12b_def.h"
#include "rf_packet.h"


class RF12B {
	public:
		RF12B();
		void begin(float band=_433MHz);
		bool packetAvailable();
		void sendPacket(byte * buf, byte length, byte id=0, uint16_t seq=0, byte type=DATA_PACKET);
		void sendPacket(RFPacket *);
		void send(byte * buf, byte length);
		RFPacket recvPacket();	
	    void setFrequency(float freq = 868.0);
		void setDatarate(uint16_t baud);
		void setOutputPower(uint8_t p);
		void setChannel(uint8_t channel);
		void setPanID(byte pan_id);
		void rxISR();
		void disableISR();
		void enableISR();
	private:
		volatile int _mode;
		bool _rfa;
		byte _remaining;
		volatile byte _state;
		volatile bool _packet_received;
		byte _recv_buffer[BUFFER_SIZE];
		byte _r_buf_pos;
		byte _pan_id;
		uint8_t _id;
		float _band;
	protected:
		void portInit();
		void rfInit() ;
		void rfSend(unsigned char data);
		bool rfAvailable();
		void FIFOReset();
		unsigned char crc8(unsigned char crc, unsigned char data);
		void changeMode(int _mode);
		unsigned int writeCmd(unsigned int cmd);
		template <typename T> uint16_t writeCmd(T c) {return writeCmd(*(uint16_t *) &c);}
		struct ConfigReg {
				enum {
					B433=1,B868,B915
				};
				enum {
					LC85=0,LC90,LC95,LC100,LC105,LC110,LC115,LC120,LC125,LC130,LC135,LC140,LC145,LC150,LC155,LC160
				};
		    uint16_t load_cap      			: 4;
		    uint16_t band          			: 2;
		    uint16_t enable_fifo   			: 1;
		    uint16_t el						: 1;
		    uint16_t cmd           			: 8;   
		}_config_reg;

		struct PwrMgmtReg {
			uint16_t disable_clk     		: 1;
		    uint16_t enable_wkup_tmr 		: 1;
		    uint16_t enable_lowbat   		: 1;
		    uint16_t enable_osc      		: 1;    
		    uint16_t enable_synth    		: 1;
		    uint16_t enable_tx       		: 1;
		    uint16_t enable_bbb      		: 1;
			uint16_t enable_rx		       	: 1;
		    uint16_t cmd             		: 8;   
		}_pwr_mgmt_reg;

		#define PWR_CMD						0x8200

		struct FreqSetReg {
				uint16_t freq            	: 12;
		    uint16_t cmd             		: 4;   
		}_freq_set_reg;

		#define FREQ_CMD    		0xA000

		struct DataRateReg {
				enum {
					BAUD_115200=2,BAUD_57600=5,BAUD_19200=17,BAUD_9600=35,BAUD_4800=71
				};
				uint16_t data_rate       	: 7;
				uint16_t cs              	: 1;
		    uint16_t cmd             		: 8;   
		}_data_rate_reg;

		#define DATA_RATE_CMD 			0xC600

		struct RecvCtrlReg {
				enum {
					BBB400=1,BBB340,BBB270,BBB200,BBB134,BBB67
				};
				enum {
					FAST=0,MEDIUM,SLOW,ALWAYS_ON
				};
				enum {
					LNA_GAIN0=0,LNA_GAIN6,LNA_GAIN14,LNA_GAIN20
				};
				enum {
					DSSI=0,DRSSI103,DRSSI97,DRSSI91,DRSSI85,DRSSI79,DRSSI73
				};
		    uint16_t drssi_th        		: 3;    
		    uint16_t lna_gain        		: 2;
		    uint16_t baseband_bw     		: 3;
		    uint16_t vdi_resp        		: 2;
				uint16_t p16				: 1;
		    uint16_t cmd             		: 5;   
		}_recv_ctrl_reg;

		#define RECV_CTRL_CMD				0x9000

		struct DataFilterReg {   
				enum {
					DIGITAL=0,ANALOG
				};
		    uint16_t dqd_th          		: 3;
				uint16_t unknown_2       	: 1;
		    uint16_t filter_type     		: 1;
				uint16_t unknown_1       	: 1;
		    uint16_t clk_recv_fm     		: 1;
				uint16_t clk_rec_al	     	: 1;
		    uint16_t cmd             		: 8;   
		}_data_filter_reg;

		#define DFLTR_CMD			0xC200

		struct FifoRstReg {
				enum {
					SYNC2_BYTE=0,SYNC1_BYTE
				};
				enum {
					SYNC_WORD,ALWAYS
				};
		    uint16_t dis_high_sens_rst 		: 1;    
		    uint16_t enable_fifo_fill  		: 1;
		    uint16_t fifo_fill_cond    		: 1;
		    uint16_t sync_len          		: 1;
			uint16_t fifo_int_lvl      		: 4;
		    uint16_t cmd               		: 8;   
		}_fifo_reset_reg;

		#define FIFO_RST_CMD			0xCA00

		struct SyncPatReg {
		    uint16_t sync_pat				: 8;    
		    uint16_t cmd               		: 8;   
		}_sync_pat_reg;	

		#define SET_SYNC_PAT(x)     (0x6700 | x)

		#define READ_FIFO						0xB000

		struct AFCReg {
		  	uint16_t afc_enable        		: 1;
		    uint16_t afc_output_reg    		: 1;    
		    uint16_t afc_high_acc      		: 1;
		    uint16_t st_store_output   		: 1;
		    uint16_t range_lim         		: 2;
				uint16_t afc_auto		    : 2;
		    uint16_t cmd               		: 8;   
		}_afc_reg;

		struct TXConfReg {
		    uint16_t out_power       		: 3;
				uint16_t unknown_1       	: 1;
		    uint16_t freq_dev        		: 4;
				uint16_t mod_pol         	: 1;
		    uint16_t cmd             		: 7;   
		}_tx_conf_reg;

		#define TX_CONF_REG					0x9800

		struct PLLSetReg {
				uint16_t pll_bandwidth   	: 1;
				uint16_t unknown_2       	: 1;
				uint16_t disable_dit     	: 1;
		    	uint16_t ph_det_delay    	: 1;
				uint16_t unknown_1       	: 1;
				uint16_t clk_freq        	: 2;
		    uint16_t cmd             		: 9;   
		}_pll_set_reg;

		#define TX_REG_WRITE				0xB800

		struct WkupTmrReg {
		    uint16_t m			         	: 8;
				uint16_t r               	: 5;
		    uint16_t cmd             		: 3;   
		}_wkup_tmr_reg;

		struct LowDutReg {
		    uint16_t enable_low_dut  		: 8;
				uint16_t duty            	: 5;
		    uint16_t cmd             		: 8;   
		}_low_dut_reg;

		struct LowBatReg {
		    uint16_t low_bat_th      		: 4;
				uint16_t unknown_1       	: 1;
				uint16_t clk_freq        	: 3;
		    uint16_t cmd             		: 8;   
		}_low_bat_reg;
	public:
		struct StatusReg {
	      uint16_t offs0   					: 1;
	      uint16_t offs6      				: 1;    
	      uint16_t atgl					    : 1;
	      uint16_t crl	       				: 1;
	      uint16_t dqd      				: 1;
		  uint16_t rssi			       		: 1;
	      uint16_t ats		          		: 1;
		  uint16_t fifo_empty	      		: 1;
		  uint16_t lowbat				 	: 1;
		  uint16_t ext   					: 1;
		  uint16_t wkup      				: 1;    
		  uint16_t fifo_overflow    		: 1;
		  uint16_t tx_ur       				: 1;
		  uint16_t por      				: 1;
		  uint16_t fifo_full       			: 1;
		  uint16_t tx_ready         		: 1;   
		}_status_reg;
	
		struct StatusReg * status();	
};
void rxISRFunc();
extern RF12B RF12;
#endif