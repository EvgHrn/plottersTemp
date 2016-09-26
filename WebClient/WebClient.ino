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

#define maxPassDelay  7000
#define passesPerMeter 100
#define hallPin 2

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

int mainCounter = 0;
boolean inTimer = false;
int passes = 0;
float meters = 0;
int id = 0;
String startTime = "";
String stopTime = "";
int lastHallWorked = 0;


void setup() {

  pinMode(hallPin, INPUT);

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
  Serial.println("connecting...");
}

void loop() {

  if (digitalRead(hallPin) == LOW) {
    hall_worked();
  }

  if (((millis() - lastHallWorked) > maxPassDelay) && (inTimer == true)) {
    stopPrintSession();
  }

  // if there are incoming bytes available
  // from the server, read them and print them:
  if (client.available()) {
  }

  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    //Serial.println();
    //Serial.println("disconnecting.");
    //client.stop();

    // do nothing forevermore:
    //while (true);
  }
}

void sendDB(int _id, byte _plotter, String _startTime, String _stopTime, int _passes, int _meters) {
  String post = String("id=") + _id + String("&plotter=") + _plotter + String("&startTime=") + _startTime + String("&stopTime=") + _stopTime + String("&passes=") + _passes + String("&meters=") + _meters;
  int content_length = post.length();
  if (client.connect(server, 3000)) {
    Serial.println("connected to server");
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
  } else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }
}

void hall_worked() {
  Serial.println("Hall worked");
  lastHallWorked = millis();
  if (inTimer == false) {
    Serial.println("We started print session");
    inTimer = true;
    passes = 0;
    meters = 0;
    DateTime now = rtc.now();
    startTime = String(now.year()) + String("-") + String(now.month()) + String("-") + String(now.day()) + String(" ") + String(now.hour()) + String(":") + String(now.minute());//startTime=2016-09-09T12%3A04&stopTime=2016-10-01T12%3A12
  } else {
    // if we are in timer
    lastHallWorked = millis();
    passes++;
    Serial.print("Passes: ");
    Serial.println(passes);
  }
  delay(500);
}

void stopPrintSession() {
  Serial.println("We are in stopSession procedure");
  inTimer = false;
  DateTime now = rtc.now();
  stopTime = String(now.year()) + String("-") + String(now.month()) + String("-") + String(now.day()) + String(" ") + String(now.hour()) + String(":") + String(now.minute());
  meters = passes / passesPerMeter;
  sendDB(id++, 2, startTime, stopTime, passes, meters);
  passes = 0;
  meters = 0;
  startTime = "";
  stopTime = "";
}
