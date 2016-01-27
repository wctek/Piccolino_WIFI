/*************************************************************
* This library supports the ESP8266 WIFI device on the new   *
* Piccolino micro controller                 *
*                                                            *
* **** THIS IS A VERY EARLY RELEASE! IF YOU CAN IMPROVE IT   *
* **** PLEASE DO SO AND SEND US A COPY: admin@wctek.com      *
*                                                            *
*  1 Jan 15   - v0.10 Initial version                        *
* 12 Mar 15   - v0.20 Compatibility with piccolino rev5      *
* 22 Sep 15   - v0.30 Added getFeed function for piccolino.io*
*                     rewritten in getResponse();            *
*                     added callFunction() thanks Raja Balu  *
*                                                            *
*************************************************************/

#include "Arduino.h"
#include <SoftwareSerial.h>
#include <Piccolino_RAM.h>

#ifndef _PICCOLINO_WIFI_H
#define _PICCOLINO_WIFI_H

#define STATION 0
#define AP 1

#define ADDR_SERIALBUFF	0x7400 // 2K of buffer since video buffer stars at 0x7c00

class Piccolino_WIFI {
public:

  Piccolino_WIFI();
  ~Piccolino_WIFI();
  void begin(); 
  void enable();
  void disable();
  void startServer();
  char Connect(char *SSID, char *PWD);
  void setIP(char *IP, char *MASK, char *GW);
  void getFeed(char *key, char *email, char *feed_id, char *options);
  void load(char *fname);
  void getIP();
  void getResponse();
  void disconnect();
  void callFunction(char *fname);

  
  virtual size_t write(uint8_t byte);
  virtual int read();
  virtual int available();
  virtual void flush();
  
private:
  void _flush();
  void _wait(int len);
  void _clean_buffer();
  byte local_buff[128];
};

#endif