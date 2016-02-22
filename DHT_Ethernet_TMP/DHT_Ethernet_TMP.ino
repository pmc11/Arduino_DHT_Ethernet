#include "DHT.h"
 
// Pin for the DHT sensor
#define DHTPIN 7    
#define DHTTYPE DHT22
// #define DHTTYPE DHT11
 
// Create instance for the DHT sensor
DHT dht(DHTPIN, DHTTYPE);

#include <EtherCard.h>
static byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };
static byte myip[] = { 192,168,20,2 };
byte Ethernet::buffer[500];
BufferFiller bfill;


// Setup
void setup(void)
{
  dht.begin();
  Serial.begin(115200);
	if (ether.begin(sizeof Ethernet::buffer, mymac) == 0)
		Serial.println( "Failed to access Ethernet Controller");
	ether.staticSetup(myip);
}
 
static word homePage(){
	long t=millis() / 1000;
	word h = t / 3600;
	byte m = (t / 60) % 60;
	byte s = t % 60;
	bfill = ether.tcpOffset();
	bfill.emit_p(PSTR(
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: text/hmtl\r\n"
//		"Pragma: no-cache\r\n"
//		"Connection: close\r\n"
//		"Refresh: 5\r\n"
		"\r\n"
//		"<!DOCTYPE HTML>"
		"<html>"
		"<title>Temp Server</title>"
		"<h1>$D$D:$D$D:$D$D<h1>"
		"</html>"),
		h/10, h%10, m/10, m%10, s/10, s%10);
	return bfill.position();
}


void loop(void)
{

word len = ether.packetReceive();
word pos = ether.packetLoop(len);

if (pos) //check if valid tcp data is received
	ether.httpServerReply(homePage()); //send web page data


    // Get command
    if (Serial.available()) {
 
      // Read command
      byte c = Serial.read ();
 
      // If a measurement is requested, measure data and send it back
      if (c == 'm'){
 
          int humidity = (int)dht.readHumidity();
          int temp = (int)dht.readTemperature();
 
          // Send data (temperature,humidity)
          Serial.println(String(temp) + "," + String(humidity));
      }
  }
}
