
#include <Wire.h>
#include "RTClib.h"
#include <SPI.h>
#include <Ethernet.h>
#include "Arduino.h"

#define plotterNumber  5
#define maxPassDelay  8000
#define passesPerMeter 80
#define passLedPin 5
#define errTCPLedPin 6
#define errRTCLedPin 7
#define intPin 3

byte mac[] = { 0xDA, 0xAE, 0xBE, 0xEF, 0xFE, 0xED };

IPAddress server(185, 154, 12, 69); // numeric IP for Google (no DNS)

// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(192, 168, 0, 177);

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client;

RTC_DS1307 rtc;

boolean inTimer = false;
unsigned int passes = 0;
float meters = 0.0;
unsigned long id = 0;
String startTime = "";
String stopTime = "";
unsigned long lastHallWorked = 0;
volatile boolean isHall = false;


void setup() {

  pinMode(passLedPin, OUTPUT);
  pinMode(errTCPLedPin, OUTPUT);
  pinMode(errRTCLedPin, OUTPUT);
  digitalWrite(passLedPin, LOW);
  digitalWrite(errTCPLedPin, LOW);
  digitalWrite(errRTCLedPin, LOW);

  pinMode(intPin, INPUT_PULLUP);

  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  if (! rtc.begin()) {
    Serial.println(F("NoRTC"));
    digitalWrite(errRTCLedPin, HIGH);
    while (1);
  }

  if (! rtc.isrunning()) {
    Serial.println(F("RTCerr"));
    digitalWrite(errRTCLedPin, HIGH);
    // following line sets the RTC to the date & time this sketch was compiled
    //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    Serial.println(F("errEth"));
    digitalWrite(errTCPLedPin, HIGH);

    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip);
  }
  // give the Ethernet shield a second to initialize:
  delay(1000);

  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  attachInts();

  Serial.println(F("SetupF"));
  delay(1000);

  interrupts();
}

void loop() {

  if (isHall) {
    digitalWrite(passLedPin, HIGH);
    hall_worked();
    isHall = false;
  }

  if ((inTimer == true) && ((millis() - lastHallWorked) > maxPassDelay) ) {
    stopPrintSession(plotterNumber);
  }
}

void sendDB(int _id, byte _plotter, String _startTime, String _stopTime, int _passes, float _meters) {
  //Serial.println(freeRam());
  detachInts();
  String post = String("id=") + _id + String("&plotter=") + _plotter + String("&startTime=") + _startTime + String("&stopTime=") + _stopTime + String("&passes=") + _passes + String("&meters=") + _meters;
  //Serial.println(freeRam());
  if (client.connect(server, 3000)) {
    digitalWrite(errTCPLedPin, LOW);
    Serial.println(post);
    // Make a HTTP request:
    client.println("POST /quotes HTTP/1.1");
    client.println("Host: 185.154.12.69:3000");
    client.println("Cache-Control: no-cache");
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.print("Content-Length: ");
    client.println(post.length());
    client.println();
    client.println(post);
    client.stop();
  } else {
    // if you didn't get a connection to the server:
    Serial.println(F("errSend"));
    digitalWrite(errTCPLedPin, HIGH);
  }
  attachInts();
}

void hall_worked() {
  //Serial.println(freeRam());
  lastHallWorked = millis();
  if (inTimer == false) {    //if we are out of timer
    inTimer = true;
    passes = 1;
    meters = 0;
    detachInts();
    startTime = getTime();
    attachInts();
    Serial.print(F("Pss: "));
    Serial.println(passes);
  } else {
    // if we are in timer
    passes++;
    Serial.print(F("Pss: "));
    Serial.println(passes);
  }
  digitalWrite(passLedPin, LOW);
}

void stopPrintSession(int pltoStop) {
  id++;
  inTimer = false;
  detachInts();
  stopTime = getTime();
  attachInts();
  meters = passes / float(passesPerMeter);
  if (passes > 1) {
    sendDB(id, pltoStop, startTime, stopTime, passes, meters);
  }
  startTime = "";
  stopTime = "";
  meters = 0;
  passes = 0;
  //Serial.println(freeRam());
}

String getTime() {
  DateTime now = rtc.now();
  return String(String(now.year()) + "-" + String(now.month()) + "-" + String(now.day()) + " " + String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second()) + String("+4"));
}

void intHall(){
  isHall = true;
}

int freeRam () {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

void attachInts(){
  attachInterrupt(digitalPinToInterrupt(intPin), intHall, FALLING);
}

void detachInts(){
  detachInterrupt(digitalPinToInterrupt(intPin));
}
