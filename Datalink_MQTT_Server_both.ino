/***************************************************
  Voice controlled B&O Beogram via datalink with IFTTT and Adafruit io
  https://ifttt.com/
  https://io.adafruit.com
 ****************************************************/
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

// the value in bgcmd topic turns the led on/off - 0=off any other value=on
#define LED 2  // EPS8266 GPIO2
// Set web server port number to 80
WiFiServer server(80);

/************************* WiFi Access Point *********************************/

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  8883                                  // 1883 for http 8883 for https
#define AIO_USERNAME    "your Adafruit io username"           // Adafruit io username
#define AIO_KEY         "your Adafruit io key"       // Adafruit io key

/************ Global State ***************************************************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
//WiFiClient client;  // Must set AIO_SERVERPORT to 1883
// or... use WiFiFlientSecure for SSL
WiFiClientSecure client;  // Must set AIO_SERVERPORT to 8883

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/

// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
// Setup a feed called 'bgcmd' for publishing.
Adafruit_MQTT_Publish bgcmd = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/bgcmd");

// Setup a feed called 'bgcmd' for subscribing.
Adafruit_MQTT_Subscribe BeoGramCmd = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/bgcmd");

/*************************** Sketch Code ************************************/

// Bug workaround for Arduino 1.6.6, it seems to need a function declaration
// for some reason (only affects ESP8266, likely an arduino-builder bug).
void MQTT_connect();
// Variable to store the HTTP request
String header;
int pub_delay = 50;

//Serial Hex code for Beogram control:

int ply = 0x35; //Phono/Play    00x35
int nxt = 0x2b; //Next          00x2b
int prv = 0x18; //Previous      00x18
int stp = 0x26; //Pause/Stop    00x26
int sby = 0x16; //Stand-by      00x16

void setup() {
  //WiFiManager
  WiFiManager wifiManager;
  //reset saved settings
  //wifiManager.resetSettings();
  //fetches ssid and pass from eeprom and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "BeoGram"
  //and goes into a blocking loop awaiting configuration

  //set callback that gets called when connecting to previous WiFi fails
  //and enters Access Point mode
  //wifiManager.setAPCallback(configModeCallback);

  wifiManager.autoConnect("BeoGram", "admin");
  //or use this for auto generated name ESP + ChipID
  //wifiManager.autoConnect();
  pinMode(LED, OUTPUT);

  //Serial Hex code for Beogram control:
  //Phono/Play    00x35
  //Next          00x2b
  //Previous      00x18
  //Pause/Stop    00x26
  //Stand-by      00x16

  Serial.begin(320, SERIAL_7N1);    //set baud rate to match the speed on datalink
  delay(10);

  // Setup MQTT subscription for OnOff feed.
  mqtt.subscribe(&BeoGramCmd);
  //server begin
  server.begin();
}

uint32_t x = 0;

void Dlink_Tx (int Hex_Cmd) {
  int var = 0;
  while (var < 2) {
    Serial.write(Hex_Cmd);
    delay(50);
    var++;
  }
}

void beobg_pub() {
  pub_delay--;
  if (pub_delay == 0) {
    bgcmd.publish(0);
    pub_delay = 100;
  }
}

void loop() {

  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected)
  MQTT_connect();

  // this is our 'wait for incoming subscription packets' busy subloop
  // try to spend your time here

  WiFiClient client = server.available();   // Listen for incoming clients
  // HTTP server
  if (client) {                             // If a new client connects,
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            // send datalink data as serial code (HEX)
            //Code test to Overture: Phono 0x1E
            //code to BeoGram:
            //Phono 0x35
            //Ph next 0x2B
            //Previous 0x18
            //Stby 0x16
            //Stop 0x26

            // Test via HTTP GET
            // http://IP-addresss/bg65/phono
            // http://IP-addresss/bg65/next
            // http://IP-addresss/bg65/prev
            // http://IP-addresss/bg65/stop
            // http://IP-addresss/bg65/stby



            if (header.indexOf("GET /bg65/phono") >= 0) {
              digitalWrite(LED, LOW);
              Dlink_Tx(ply);

            } else if (header.indexOf("GET /bg65/next") >= 0) {
              digitalWrite(LED, LOW);
              Dlink_Tx(nxt);

            } else if (header.indexOf("GET /bg65/prev") >= 0) {
              digitalWrite(LED, LOW);
              Dlink_Tx(prv);

            } else if (header.indexOf("GET /bg65/stop") >= 0) {
              digitalWrite(LED, LOW);
              Dlink_Tx(stp);

            } else if (header.indexOf("GET /bg65/stby") >= 0) {
              digitalWrite(LED, LOW);
              Dlink_Tx(sby);

            }


            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    digitalWrite(LED, HIGH);
    // Close the connection
    client.stop();
  }



  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(50))) {
    // Check if its the bgcmd feed
    if (subscription == &BeoGramCmd) {
      if (strcmp((char *)BeoGramCmd.lastread, "0") == 0) {
        digitalWrite(LED, HIGH);
      }
      //if value 1 send Phono command
      else if (strcmp((char *)BeoGramCmd.lastread, "1") == 0) {
        digitalWrite(LED, LOW);
        //Serial.println("phono/play");
        Dlink_Tx(ply);
      }
      // if value 2 send Next command
      else if (strcmp((char *)BeoGramCmd.lastread, "2") == 0) {
        digitalWrite(LED, LOW);
        //Serial.println("next");
        Dlink_Tx(nxt);
      }
      // if value 3 send Prev command
      else if (strcmp((char *)BeoGramCmd.lastread, "3") == 0) {
        digitalWrite(LED, LOW);
        //Serial.println("prev");
        Dlink_Tx(prv);
      }
      // if value 4 send Stop command
      else if (strcmp((char *)BeoGramCmd.lastread, "4") == 0) {
        digitalWrite(LED, LOW);
        //Serial.println("stop");
        Dlink_Tx(stp);
      }
      // if value 5 send Stand-by command
      else if (strcmp((char *)BeoGramCmd.lastread, "5") == 0) {
        digitalWrite(LED, LOW);
        //Serial.println("stby");
        Dlink_Tx(sby);
      }

      else
      {
        //do nothingh! Continue usual tasks...
      }
    }
  }

  // Now we can publish stuff and reset the value of the bgcmd topic to 0
  //bgcmd.publish(0);
  beobg_pub();
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
    mqtt.disconnect();
    delay(5000);  // wait 5 seconds
    retries--;
    if (retries == 0) {
      // basically die and wait for WDT to reset me
      while (1);
    }
  }
}

