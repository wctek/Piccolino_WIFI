/*************************************************************
* This library supports the ESP8266 WIFI device on the new   *
* Piccolino micro controller								 *
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

#include "Piccolino_WIFI.h"

SoftwareSerial _pserial(7,6); // coms channel to the ESP8266
Piccolino_RAM  _p_ram; // serial buffer in SRAM

Piccolino_WIFI::Piccolino_WIFI() {
	// all done in 'begin'
}


void Piccolino_WIFI::_flush(){
	for(char f=0; f<3; f++) {
		while(_pserial.available()) { // flush the buffer
			_pserial.flush();
			_pserial.read();
		}
		delay(100);
	}
}

void Piccolino_WIFI::callFunction(char *fname) {
	_pserial.println(fname);

}

void Piccolino_WIFI::send(String str) {
	_pserial.print(str);

}

void Piccolino_WIFI::load(char *fname) {
	String cmd;
	cmd="dofile(\"";
	cmd+=fname;
	cmd+="\")";

	_pserial.println(cmd);
}

void Piccolino_WIFI::getResponse() {
	char pos=0;
	int addr=0;
	char ch;

	while(1) {
		if(_pserial.available()) {
			ch=_pserial.read();
			if(ch!='>') { // slippery little suckers

				if(ch==13) {									
					local_buff[pos]=0;
					_p_ram.write(addr,local_buff,pos+1);
					addr+=pos;
					pos=0;

					if(strlen((char *)local_buff)>1) // skip trash ...
						return;
				}

				local_buff[pos]=ch;
				pos++;
				if(pos==1&&ch==32) // skip initial space that may exist in a string (I know it's a lame way to do this)
					pos--;
				if(pos>126) {
					local_buff[pos]=0;
					_p_ram.write(addr,local_buff,pos+1);
					addr+=pos;
					pos=0;
				}
			}
		}		
	}
}

void Piccolino_WIFI::disconnect() {
	_pserial.println(F("disc()"));
}

void Piccolino_WIFI::_wait(int len) {
	int st=millis();
	st+=len;

	while(millis()<st) {
		_pserial.peek();
	}
}

void Piccolino_WIFI::_clean_buffer() {
	for(int i = 0; i < (sizeof(local_buff)/sizeof(*local_buff)); i++) {
		local_buff[i] = 0;
	}
}

void Piccolino_WIFI::setIP(char *IP, char *MASK, char *GW) {

	String cmd;
	cmd+="setIP(\"";
	cmd+=IP;
	cmd+="\",\"";
	cmd+=MASK;
	cmd+="\",\"";
	cmd+=GW;
	cmd+="\")";

	_flush();
	_pserial.println(cmd);
	_wait(100);
}

void Piccolino_WIFI::getFeed(char *key, char *email, char *feed_id, char *options) {

	String cmd;
	cmd+="getFeed(\"";
	cmd+=key;
	cmd+="\",\"";
	cmd+=email;
	cmd+="\",\"";
	cmd+=feed_id;
	cmd+="\",\"";
	cmd+=options;
	cmd+="\")";

	_flush();
	_pserial.println(cmd);
	_wait(100); 
}

void Piccolino_WIFI::getIP() {
	char ch,oldch,failed=0;
	char pos=0;

	_pserial.println(F("getIP()"));

	_wait(500);
	
	// Set the failure condition
	local_buff[0] = '.';
	_p_ram.write(0,local_buff,1);
	
	_clean_buffer();

	while(failed <= 20) {
		if(_pserial.available()) {
			ch=_pserial.read();

			if(ch==13) {				
				if(oldch!='.') { // skip the '.' which indicates waiting for ip ...
					_p_ram.write(0,local_buff,pos);
					return;			
				} else {
					_clean_buffer();
					pos=0;
					failed++;
				}
			}
			else
			{
				oldch=ch;
				local_buff[pos]=ch;
				pos++;
				if(pos>15) {
					_clean_buffer();
					pos=0;
				}
			}
		}		
		_wait(100);
	}
	
}

void Piccolino_WIFI::begin() {

	enable(); // turn wifi on

	// Must switch 19200 to keep up (since were' using softserial)
	_pserial.begin(9600);
	_p_ram.begin(ADDR_SERIALBUFF); // start at the serial buffer address ...
	memset(local_buff,0,17);

	for(int f=0; f<100; f+=17) // clear the first 170 bytes
		_p_ram.write(f,local_buff,17);
	
	_flush();
	_pserial.println("\n\n");
	_flush();
	_pserial.println(F("uart.setup(0,19200,8,0,1,0)"));
	_pserial.begin(19200);
	_flush();
}

void Piccolino_WIFI::enable() {
	pinMode(A3,OUTPUT);
	digitalWrite(A3,HIGH);
}

void Piccolino_WIFI::disable() {
	pinMode(A3,OUTPUT);
	digitalWrite(A3,LOW);
}

int Piccolino_WIFI::read() {
	return(_pserial.read());
}

size_t Piccolino_WIFI::write(uint8_t byte) {
	_pserial.write(byte);
}
int Piccolino_WIFI::available() {
	return(_pserial.available());
}
void Piccolino_WIFI::flush() {
	_pserial.flush();
}

char Piccolino_WIFI::Connect(char *SSID,char *PWD) {

	String cmd;
	cmd+="con(\"";
	cmd+=SSID;
	cmd+="\",\"";
	cmd+=PWD;
	cmd+="\")";

	_flush();
	_pserial.println(cmd);

	getIP();	
}

void Piccolino_WIFI::startServer() {
	_flush();
	_pserial.println(F("startServer()"));
}

Piccolino_WIFI::~Piccolino_WIFI() {
	// nothing to destroy (yet)
}
