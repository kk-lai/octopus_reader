# octopus_reader
Read Octopus Balance with Arduino Nano + PN532 NFC RFID module

	Connection Diagram:
	
	  Nano             PN532 b
	+------+          +------+
	+  +5V +----------+ VCC  +
	+  GND +----------+ GND  +  
	+   A5 +----------+ SCL  +  
	+   A4 +----------+ SDA  +
	+------+          +------+

# Description
The software initiate a InListPassiveTarget command to poll for an octopus card, then it send a "Read Without Encryption" command to the card with service code 0x117 to read 16 bytes from the card.
The first 4 bytes (big endian) contain the adjusted balance.

The balance can be calculated by 

(adjusted balance - 350)/10

# Dependence
https://github.com/elechouse/PN532
Folder NDEF, PN532, PN532_I2C should be copied to libraries folder of Arduino

# Note
1. I have to modify the buffer size of "Wire.h" and "twi.h" from 32 to 64, in order to make the program work
2. I tested with several octopus cards and octopus watches, but the program unable read some of them.


