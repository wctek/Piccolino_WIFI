/*
Connects to Piccolino.io and retrieves a feed (CC record)
*/

const char *MY_SSID="your ssid";
const char *MY_PASS="your networkpwd";
const char *KEY="your key";
const char *EMAIL="your email";
const char *FEED_ID="your feed no";
const char *OTH="";

 
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
  
  byte local_buff[128]; // just to show our IP
  byte t=120;
  display.clear();
  display.setTextSize(1);
  display.setCursor(0,0);
  
  debug(F("Loading script ...\n")); 
  wifi.load("picoFeed.lua");

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
          
    debug(F("Getting Feed...\n"));
  
    // now we can process the input and act upon it

    while(1){
      delay(1000);
      if(t++>=120) {
        wifi.getFeed((char *)KEY, (char *)EMAIL, (char *)FEED_ID, (char *)OTH);
        t=0;
      }
      
      if(wifi.available()) {    
        wifi.getResponse(); // places response in the serial buffer
        ram.read(ADDR_SERIALBUFF,local_buff,128);
        display.clear();
        display.setCursor(0,0);
        Serial.println((char *)local_buff);        
        
        display.setTextSize(2);
        display.print("Temp: ");
        if(local_buff[20]!=48) // as long as it's not a leading zero
          display.print((char)local_buff[20]);
        display.print((char)local_buff[21]); // tenth of degree F
        display.print((char)local_buff[22]); // unit of degree F
        display.println(" F");        
        display.setCursor(0,20);
        if(local_buff[2]=='C'&&local_buff[3]=='L') // CLR= CLEAR SKY
          display.print("CLEAR SKY");
        else if((local_buff[2]=='F'&&local_buff[3]=='E') || (local_buff[2]=='S'&&local_buff[3]=='C')) // FEW|SCT = FEW CLOUDS
          display.print("FEW CLOUDS");
        display.update();       
      }  
    }
  }
}
