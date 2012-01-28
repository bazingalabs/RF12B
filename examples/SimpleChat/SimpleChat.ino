#include <SPI.h>
#include <RF12B.h>
#include <rf_packet.h>

void setup() {	
	Serial.begin(57600);
	RF12.begin();

	Serial.println("Simple Chat started");
}
byte buf[50];
int i = 0;
void loop() {
	if (Serial.available()>0) {
		// Read from serial
		char c = Serial.read();
		buf[i++] = c;
		if (c == '\n') {
			RFPacket packet(buf, i, 1, 1,2);
			RF12.send(packet, packet.size()); 
			// Clear rest of buffer for easy printing
			memset(&buf[i-1],'\0',50-i);
			buf[i+1] = '\0';
			Serial.println("Sent:");
			Serial.println((char *)buf);
			i=0;
		}
	}
	if (RF12.packetAvailable()) {
		RFPacket p = RF12.recvPacket();
		// Only print valid packets
		if (p.valid()) {
			Serial.println("Received:");
			Serial.print(p);
		}
	}
}
