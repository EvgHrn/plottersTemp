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

#define maxPassDelay  7000
#define passesPerMeter 77
#define hall1Pin 2
#define ledPin 3
#define hall2Pin 4

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

unsigned int mainCounter = 0;
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
const int maxAllowedWrites = 800;
const int memBase = 350;
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
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
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
  Serial.println("Setup section done");
}

void loop() {

  if (digitalRead(hall1Pin) == LOW) {
    hall_worked(1);
  }

  //if (digitalRead(hall2Pin) == LOW) {
   // hall_worked(2);
  //}

  if ((inTimer1 == true) && ((millis() - lastHallWorked1) > maxPassDelay) ) {
    Serial.print("Too long delay on 1: ");
    Serial.print("millis: ");
    Serial.print(millis());
    Serial.print(" ");
    Serial.print("delta: ");
    Serial.println(millis() - lastHallWorked1);
    stopPrintSession(1);
  }

  if ((inTimer2 == true) && ((millis() - lastHallWorked2) > maxPassDelay) ) {
    Serial.print("Too long delay on 2: ");
    Serial.print("millis: ");
    Serial.print(millis());
    Serial.print(" ");
    Serial.print("delta: ");
    Serial.println(millis() - lastHallWorked2);
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
  String post = String("id=") + _id + String("&plotter=") + _plotter + String("&startTime=") + _startTime + String("&stopTime=") + _stopTime + String("&passes=") + _passes + String("&meters=") + _meters;
  int content_length = post.length();
  if (client.connect(server, 3000)) {
    Serial.println("We are connected to server");
    Serial.print("Request: ");
    Serial.println(post);
    // Make a HTTP request:
    client.println("POST /quotes HTTP/1.1");
    client.println("Host: 5.63.159.247:3000");
    client.println("Cache-Control: no-cache");
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.print("Content-Length: ");
    client.println(content_length);
    client.println();
    client.println(post);
    client.stop();
  } else {
    // if you didn't get a connection to the server:
    Serial.println("Connection failed");
  }
}

void hall_worked(int pl) {
  digitalWrite(ledPin, HIGH);
  Serial.print("Hall ");
  Serial.print(pl);
  Serial.println(" worked");
  switch (pl) {
    case 1: {
      lastHallWorked1 = millis();
      Serial.print("lastHallWorked_1: ");
      Serial.println(lastHallWorked1);
      if (inTimer1 == false) {    //if we are out of timer
        Serial.println("We started print session on 1 plotter");
        inTimer1 = true;
        passes1 = 1;
        meters1 = 0;
        startTime1 = getTime();
      } else {
        // if we are in timer
        passes1++;
        Serial.print("Passes 1: ");
        Serial.println(passes1);
      }
    }
    case 2: {
      lastHallWorked2 = millis();
      Serial.print("lastHallWorked_2: ");
      Serial.println(lastHallWorked2);
      if (inTimer2 == false) {    //if we are out of timer
        Serial.println("We started print session on 2 plotter");
        inTimer2 = true;
        passes2 = 1;
        meters2 = 0;
        startTime2 = getTime();
      } else {
        // if we are in timer
        passes2++;
        Serial.print("Passes 2: ");
        Serial.println(passes2);
      }
    }
  }
  delay(500);
  digitalWrite(ledPin, LOW);
}

void stopPrintSession(int pl) {
  Serial.print("We are in stopSession ");
  Serial.print(pl);
  Serial.println(" procedure");
  switch (pl){
    case 1: {
      inTimer1 = false;
      stopTime1 = getTime();
      meters1 = passes1 / float(passesPerMeter);
      Serial.print("Passes 1: ");
      Serial.print(passes1);
      Serial.print(". ");
      Serial.print("Meters 1: ");
      Serial.println(meters1);
      sendDB(id, 1, startTime1, stopTime1, passes1, meters1);
      startTime1 = "";
      stopTime1 = "";
    }
    case 2: {
      inTimer2 = false;
      stopTime2 = getTime();
      meters2 = passes2 / float(passesPerMeter);
      Serial.print("Passes 2: ");
      Serial.print(passes2);
      Serial.print(". ");
      Serial.print("Meters 2: ");
      Serial.println(meters2);
      sendDB(id, 2, startTime2, stopTime2, passes2, meters2);
      startTime2 = "";
      stopTime2 = "";
    }
  }
  id = readId();
  id++;
  writeId(id);
}

String getTime(){
  DateTime now = rtc.now();
  return String(String(now.year()) + String("-") + String(now.month()) + String("-") + String(now.day()) + String(" ") + String(now.hour()) + String(":") + String(now.minute()));
}

void writeId(unsigned long _id){
  EEPROM.writeLong(addressLong, _id);
  Serial.print("Id ");
  Serial.print(_id);
  Serial.println(" writed.");
}

unsigned long readId(){
  unsigned long ee_id = EEPROM.readLong(addressLong);
  Serial.print("Id read: ");
  Serial.println(ee_id);
  return ee_id;
}
