#include <SPI.h>
#include <RF12B.h>
#include <rf_packet.h>

void setup() {
	Serial.begin(57600);
	RF12.begin();
	
	Serial.println("Started");
}

byte buf[50];
int i = 0;
void loop() {
	// Read from serial
	if (Serial.available()>0) {
		char c = Serial.read();
		buf[i++] = c;
		// On newline found start sending
		if (c == '\n') {
			RF12.sendPacket(buf, i);
			buf[i+1] = '\0';
			Serial.print("Sent: ");
			Serial.println((char *)buf);
			i=0;
		}
	}
	// If a rf package is available print it
	if (RF12.packetAvailable()) {
		RFPacket p = RF12.recvPacket();
		p.dump();
	}
}

