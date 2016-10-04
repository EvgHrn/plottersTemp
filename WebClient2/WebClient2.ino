/*
  Web client

  This sketch connects to a website (http://www.google.com)
  using an Arduino Wiznet Ethernet shield.

  Circuit:
   Ethernet shield attached to pins 10, 11, 12, 13

  created 18 Dec 2009
  by David A. Mellis
  modified 9 Apr 2012
  by Tom Igoe, based on work by Adrian McEwen

*/
#include <Wire.h>
#include "RTClib.h"
#include <SPI.h>
#include <Ethernet.h>
#include <EEPROMex.h>
#include "Arduino.h"

#define maxPassDelay  8000
#define passesPerMeter 80
#define hall1Pin 2
#define ledPin 3
#define hall2Pin 5
#define maxAllowedWrites 10000
#define memBase 350

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
IPAddress server(5, 63, 159, 247); // numeric IP for Google (no DNS)

// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(192, 168, 0, 177);

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client;

RTC_DS1307 rtc;

boolean inTimer1 = false;
boolean inTimer2 = false;
unsigned int passes1 = 0;
float meters1 = 0.0;
unsigned int passes2 = 0;
float meters2 = 0.0;
unsigned long id;
String startTime2 = "";
String stopTime2 = "";
String startTime1 = "";
String stopTime1 = "";
unsigned long lastHallWorked1 = 0;
unsigned long lastHallWorked2 = 0;
int addressLong;


void setup() {
  pinMode(hall1Pin, INPUT);
  pinMode(hall2Pin, INPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  if (! rtc.begin()) {
    Serial.println("NoRTC");
    while (1);
  }

  if (! rtc.isrunning()) {
    Serial.println("RTC stoped");
    // following line sets the RTC to the date & time this sketch was compiled
    //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Eth. failed");
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip);
  }
  // give the Ethernet shield a second to initialize:
  delay(1000);

  EEPROM.setMemPool(memBase, EEPROMSizeUno);
  EEPROM.setMaxAllowedWrites(maxAllowedWrites);

  delay(100);
  addressLong = EEPROM.getAddress(sizeof(long));

  //writeId(0); //just the first time to initialize id
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  Serial.println("Setup done");
}

void loop() {

  if (digitalRead(hall1Pin) == LOW) {
    hall_worked(1);
  }

  if (digitalRead(hall2Pin) == LOW) {
    hall_worked(2);
  }

  if ((inTimer1 == true) && ((millis() - lastHallWorked1) > maxPassDelay) ) {
    Serial.println("Long delay 1");
    stopPrintSession(1);
  }

  if ((inTimer2 == true) && ((millis() - lastHallWorked2) > maxPassDelay) ) {
    Serial.println("Long delay 2");
    stopPrintSession(2);
  }

  // if there are incoming bytes available
  // from the server, read them and print them:
  //if (client.available()) {
  //}

  // if the server's disconnected, stop the client:
  //if (!client.connected()) {
  //Serial.println();
  //Serial.println("disconnecting.");
  //client.stop();
  // do nothing forevermore:
  //while (true);
  //}
}

void sendDB(int _id, byte _plotter, String _startTime, String _stopTime, int _passes, float _meters) {
  String post = "id=" + _id + String("&plotter=") + _plotter + String("&startTime=") + _startTime + String("&stopTime=") + _stopTime + String("&passes=") + _passes + String("&meters=") + _meters;
  if (client.connect(server, 3000)) {
    Serial.println("ConnectedToServer");
    Serial.print("Req: ");
    Serial.println(post);
    // Make a HTTP request:
    client.println("POST /quotes HTTP/1.1");
    client.println("Host: 5.63.159.247:3000");
    client.println("Cache-Control: no-cache");
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.print("Content-Length: ");
    client.println(post.length());
    client.println();
    client.println(post);
    client.stop();
  } else {
    // if you didn't get a connection to the server:
    Serial.println("SendFailed");
  }
}

void hall_worked(int pl) {
  digitalWrite(ledPin, HIGH);
  Serial.print("Pl ");
  Serial.println(pl);
  switch (pl) {
    case 1: {
        lastHallWorked1 = millis();
        if (inTimer1 == false) {    //if we are out of timer
          Serial.println("Print 1");
          inTimer1 = true;
          passes1 = 1;
          meters1 = 0;
          startTime1 = getTime();
          Serial.println(startTime1);
        } else {
          // if we are in timer
          passes1++;
          Serial.print("Passes 1: ");
          Serial.println(passes1);
        }
      }
      break;
    case 2: {
        lastHallWorked2 = millis();
        if (inTimer2 == false) {    //if we are out of timer
          Serial.println("Print 2");
          inTimer2 = true;
          passes2 = 1;
          meters2 = 0;
          startTime2 = getTime();
          Serial.println(startTime2);
        } else {
          // if we are in timer
          passes2++;
          Serial.print("Passes 2: ");
          Serial.println(passes2);
        }
      }
      break;
    default: {
        Serial.println("ErrorSwitch");
      }
      break;
  }
  delay(250);
  digitalWrite(ledPin, LOW);
}

void stopPrintSession(int pltoStop) {
  Serial.print("Stop ");
  Serial.println(pltoStop);
  switch (pltoStop) {
    case 1: {
        inTimer1 = false;
        stopTime1 = getTime();
        meters1 = passes1 / float(passesPerMeter);
        if (passes1 > 1) {
          sendDB(id, pltoStop, startTime1, stopTime1, passes1, meters1);
        }
        startTime1 = "";
        stopTime1 = "";
      }
      break;
    case 2: {
        inTimer2 = false;
        stopTime2 = getTime();
        meters2 = passes2 / float(passesPerMeter);
        if (passes2 > 1) {
          sendDB(id, pltoStop, startTime2, stopTime2, passes2, meters2);
        }
        startTime2 = "";
        stopTime2 = "";
      }
      break;
  }
  id = readId();
  id++;
  writeId(id);
}

String getTime() {
  DateTime now = rtc.now();
  return String(String(now.year()) + "-" + String(now.month()) + "-" + String(now.day()) + " " + String(now.hour()) + ":" + String(now.minute()));
}

void writeId(unsigned long _id) {
  EEPROM.writeLong(addressLong, _id);
}

unsigned long readId() {
  unsigned long ee_id = EEPROM.readLong(addressLong);
  return ee_id;
}
