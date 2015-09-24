/*
Connects as a station to an access point and gets its own IP using DHCP
*/

const char *MY_SSID="PUT_YOUR_SSID_HERE";
const char *MY_PASS="PUT_YOUR_PWD_HERE";
 
// For the OLED, we need I2C
#include <Wire.h>

// For the SRAM we need SPI
#include <SPI.h>

// For the comms with the WIFI module we need SoftSerial
#include <SoftwareSerial.h>

// Piccolino's I2C OLED library
// store buffer in sram, slower but frees almost 1K of ram
#include <Piccolino_OLED_SRAM.h>

// store buffer in local ram, fastest but uses 1K of ram
//#include <Piccolino_OLED.h>

// Piccolino's WIFI module library
#include <Piccolino_WIFI.h>

// Piccolino's SRAM module library since we want to access the serial SRAM buffer ...
#include <Piccolino_RAM.h>

Piccolino_OLED_SRAM display; // our OLED object
Piccolino_WIFI wifi; // our WIFI object
Piccolino_RAM ram; // so that we can snoop into the WIFI buffer

void debug(const __FlashStringHelper *msg) {
  Serial.print(msg);
  display.print(msg);
  display.update();
}

void setup(void) {
  Serial.begin(9600);
  display.begin();
  delay(1000); // give time for bootloader to settle
  wifi.begin();  
  ram.begin();
}

void loop(void) {
  
  byte local_buff[17]; // just to show our IP

  display.clear();
  display.setTextSize(1);
  display.setCursor(0,0);
  
  debug(F("Loading script ...\n")); 
  wifi.load("STpicoWIFI.lua");

  wifi.disconnect();  // disconnect just in case
  debug(F("DHCP Connect ...\n"));
  wifi.Connect((char *)MY_SSID,(char *)MY_PASS); // connect - ip will be in local_buff

  ram.read(ADDR_SERIALBUFF,local_buff,16);
  if(local_buff[0]=='.'||local_buff[0]==0) {
    debug(F("FAILED to get IP! Retrying...\n"));
    delay(3000);
  } else {
    Serial.println((char *)local_buff);
    display.println((char *)local_buff);
    display.update();
    
    debug(F("Starting Server ...\n"));
    wifi.startServer();
    
    debug(F("\nWebsite should be up!\n"));

    // now we can process the input from the website and act upon it

    while(1){
      if(wifi.available()) {    
        wifi.getResponse(); // places response in the serial buffer
        ram.read(ADDR_SERIALBUFF,local_buff,16);
        display.clear();
        display.setCursor(0,0);
        debug(F("ESP8266 says:\n"));
        Serial.println((char *)local_buff);        
        display.print("\n\n");
        display.print((char *)local_buff);        
        display.update();
      }  
    }
  }
}
