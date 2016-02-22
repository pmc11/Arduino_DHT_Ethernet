#include <ethernet_comp.h>
#include <Dhcp.h>
#include <UIPEthernet.h>
#include <UIPUdp.h>
#include <UIPServer.h>
#include <Dns.h>
#include <UIPClient.h>

#include "DHT.h"
#include "SPI.h"
 
// Pin for the DHT sensor
#define DHTPIN 7    
#define DHTTYPE DHT22
// #define DHTTYPE DHT11
 
// Create instance for the DHT sensor
DHT dht(DHTPIN, DHTTYPE);

//#include <EtherCard.h>
byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };
byte myip[] = { 192,168,20,2 };
EthernetServer server(80);
long lastReadingTime = 0;
 

// Setup
void setup(void)
{
  dht.begin();
  SPI.begin();
  Ethernet.begin(mymac, myip);
  server.begin();
  Serial.begin(115200);
  delay(1000);
//	if (ether.begin(sizeof Ethernet::buffer, mymac) == 0)
//		Serial.println( "Failed to access Ethernet Controller");
//	ether.staticSetup(myip);

}


void loop(void)
{
  if (millis() - lastReadingTime > 1000) {
    listenForEthernetClients();
      // timestamp the last time you got a reading:
      lastReadingTime = millis();
    }
  }

 void listenForEthernetClients() {
  // listen for incoming clients 
 int humidity = (int)dht.readHumidity();
 int temp = (int)dht.readTemperature();
          
 EthernetClient client = server.available();
  if (client) {
    Serial.println("Got a client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println();
          // print the current readings, in HTML format:
          // Send data (temperature,humidity)
          client.print("Temperature: ");
          client.println(String(temp));
          client.print("Humidity: ");
          client.println(String(humidity));
          client.println("<br />");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
  // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
  }
 }
  

