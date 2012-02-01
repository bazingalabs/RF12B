# RF12B Library 
This is a library for the rf12b radio module from hoperf (http://www.hoperf.com)
It implements a easy to use Arduino style interface to the radio chip.
The goal is to allow Arduino's to easily communicate with each other.  
 
for examples see the examples directory

## Documentation
For documentation check the wiki https://github.com/bazingalabs/RF12B/wiki
 
## Installation
To install this library follow the steps below

1. Go to your arduino libraries directory
2. Clone the repository
<code>git clone git@github.com:bazingalabs/RF12B.git</code>
3. Restart Arduino IDE if it was running
4. The library should now be listed in the menu Sketch -> Import library and Examples under File -> Examples

## Usage
First include the rf12B, rf_packet and SPI library

```cpp
    #include <SPI.h>
    #include <RF12B.h>
    #include <rf_packet.h>
```
In setup initialize the library by calling begin optionally set things like frequency etc.

```cpp
    void setup() {	
    	Serial.begin(57600);
    	RF12.begin();
    }
```
Now send a packet by calling sendPacket

```cpp
    RFPacket packet("hello", 5, 1, 1,2);
 		 RF12.send(packet, packet.size()); 
```

And receive with

```cpp
    if (RF12.packetAvailable()) {
    	RFPacket p = RF12.recvPacket();
        // Only print valid packets
        if (p.valid()) {
    		Serial.println("Received:");
    		Serial.print(p);
    	}
    }
```

## Thanks
This is a libray for the Roger board from Bazingalabs (http://www.bazingalabs.com)  
The code is based on the rf12 tutorial from https://loee.jottit.com/rfm12b_and_avr_-_quick_start  
