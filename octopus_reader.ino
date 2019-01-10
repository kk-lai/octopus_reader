/**************************************************************************/
/*
    Read Octopus Balance with Arduino Nano + PN532 NFC RFID module
  Connection Diagram:
  
    Nano             PN532 b
  +------+          +------+
  +  +5V +----------+ VCC  +
  +  GND +----------+ GND  +  
  +   A5 +----------+ SCL  +  
  +   A4 +----------+ SDA  +
  +------+          +------+
 */
/**************************************************************************/
#include <Arduino.h>

#if 0
  #include <SPI.h>
  #include <PN532_SPI.h>
  #include <PN532.h>

PN532_SPI pn532spi(SPI, 10);
PN532 nfc(pn532spi);
#elif 0
  #include <PN532_HSU.h>
  #include <PN532.h>

PN532_HSU pn532hsu(Serial1);
PN532 nfc(pn532hsu);
#else
  #include <Wire.h>
  #include <PN532_I2C.h>
  #include <PN532.h>

PN532_I2C pn532i2c(Wire);
PN532 nfc(pn532i2c);
#endif

#include <PN532_debug.h>

uint8_t        _prevIDm[8];

void setup(void)
{
  Serial.begin(115200);
  Serial.println("Hello!");

  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata)
  {
    Serial.print("Didn't find PN53x board");
    while (1) {delay(10);};      // halt
  }

  // Got ok data, print it out!
  Serial.print("Found chip PN5"); Serial.println((versiondata >> 24) & 0xFF, HEX);
  Serial.print("Firmware ver. "); Serial.print((versiondata >> 16) & 0xFF, DEC);
  Serial.print('.'); Serial.println((versiondata >> 8) & 0xFF, DEC);

  // Set the max number of retry attempts to read from a card
  // This prevents us from waiting forever for a card, which is
  // the default behaviour of the PN532.
  nfc.setPassiveActivationRetries(0xFF);
  nfc.SAMConfig();

  memset(_prevIDm, 0, 8);
}

void loop(void)
{
  uint16_t systemCode = 0xFFFF;
  uint8_t requestCode = 0x01;       // System Code request
  uint8_t idm[8];
  uint8_t pmm[8];
  uint16_t systemCodeResponse;
  uint8_t blockData[1][16];
  uint16_t serviceCodeList[1];
  uint16_t blockList[1];
  uint8_t  ret;
  
  ret = nfc.felica_Polling(systemCode, requestCode, idm, pmm, &systemCodeResponse, 200);

  if (ret != 1)
  {
    memset(_prevIDm, 0, 8);
    return;
  }

  if ( memcmp(idm, _prevIDm, 8) != 0 ) {
      serviceCodeList[0]=0x117;
      blockList[0] = 0x8000;

      ret = nfc.felica_ReadWithoutEncryption(1, serviceCodeList, 1, blockList, blockData);
      if (ret == 1) {
        Serial.print("  IDm: ");
        nfc.PrintHex(idm, 8);
        Serial.print("  PMm: ");
        nfc.PrintHex(pmm, 8);
        Serial.print("  System Code: ");
        Serial.print(systemCodeResponse, HEX);
        Serial.print("\n");
              
        int32_t balance = 0;
        for(int i=0;i<4;i++) {
          balance<<=8;
          balance|=blockData[0][i];  
        }
        balance = balance - 350;
        Serial.print("Balance:");
        if (balance<0) {
            Serial.print("-");
            balance=-balance;
        }
        Serial.print((balance - (balance % 10))/10);
        Serial.print(".");
        Serial.println(balance % 10);
      } else {
         memset(_prevIDm, 0, 8);
      }
      //nfc.felica_Release();
      
      memcpy(_prevIDm, idm, 8);
      delay(200);
  }
  
}
