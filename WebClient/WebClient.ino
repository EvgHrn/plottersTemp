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
#define ledPin 5
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
boolean inTimer3 = false;
boolean inTimer4 = false;
boolean inTimer5 = false;
unsigned int passes1 = 0;
float meters1 = 0.0;
unsigned int passes2 = 0;
float meters2 = 0.0;
unsigned int passes3 = 0;
float meters3 = 0.0;
unsigned int passes4 = 0;
float meters4 = 0.0;
unsigned int passes5 = 0;
float meters5 = 0.0;
unsigned long id;
String startTime1 = "";
String stopTime1 = "";
String startTime2 = "";
String stopTime2 = "";
String startTime3 = "";
String stopTime3 = "";
String startTime4 = "";
String stopTime4 = "";
String startTime5 = "";
String stopTime5 = "";
unsigned long lastHallWorked1 = 0;
unsigned long lastHallWorked2 = 0;
unsigned long lastHallWorked3 = 0;
unsigned long lastHallWorked4 = 0;
unsigned long lastHallWorked5 = 0;
int addressLong;
volatile boolean isHall1 = false;
volatile boolean isHall2 = false;
volatile boolean isHall3 = false;
volatile boolean isHall4 = false;
volatile boolean isHall5 = false;


void setup() {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  if (! rtc.begin()) {
    Serial.println(F("NoRTC"));
    while (1);
  }

  if (! rtc.isrunning()) {
    Serial.println(F("RTCerr"));
    // following line sets the RTC to the date & time this sketch was compiled
    //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    Serial.println(F("errEth"));
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
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  //Serial.println("Setup done");
  interrupts();

  attachInterrupt(0, intHall1, FALLING) ;
  attachInterrupt(1, intHall2, FALLING) ;

  Serial.println(F("SetupF"));
}

void loop() {

  if (isHall1) {
    digitalWrite(ledPin, HIGH);
    hall_worked(1);
    isHall1 = false;
  }

  if (isHall2) {
    digitalWrite(ledPin, HIGH);
    hall_worked(2);
    isHall2 = false;
  }
  
  if (isHall3) {
    digitalWrite(ledPin, HIGH);
    hall_worked(3);
    isHall3 = false;
  }

  if (isHall4) {
    digitalWrite(ledPin, HIGH);
    hall_worked(4);
    isHall4 = false;
  }
  
  if (isHall5) {
    hall_worked(5);
    isHall5 = false;
  }


  if ((inTimer1 == true) && ((millis() - lastHallWorked1) > maxPassDelay) ) {
    stopPrintSession(1);
  }

  if ((inTimer2 == true) && ((millis() - lastHallWorked2) > maxPassDelay) ) {
    stopPrintSession(2);
  }

  if ((inTimer3 == true) && ((millis() - lastHallWorked3) > maxPassDelay) ) {
    stopPrintSession(3);
  }

  if ((inTimer4 == true) && ((millis() - lastHallWorked4) > maxPassDelay) ) {
    stopPrintSession(4);
  }

  if ((inTimer5 == true) && ((millis() - lastHallWorked5) > maxPassDelay) ) {
    stopPrintSession(5);
  }

  
}

void sendDB(int _id, byte _plotter, String _startTime, String _stopTime, int _passes, float _meters) {
  Serial.println(freeRam());
  noInterrupts();
  String post = String("id=") + _id + String("&plotter=") + _plotter + String("&startTime=") + _startTime + String("&stopTime=") + _stopTime + String("&passes=") + _passes + String("&meters=") + _meters;
  Serial.println(freeRam());
  if (client.connect(server, 3000)) {
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
    Serial.println(F("errSend"));
  }
  interrupts();
}

void hall_worked(int pl) {
  Serial.println(freeRam());
  switch (pl) {
    case 1: {
        lastHallWorked1 = millis();
        if (inTimer1 == false) {    //if we are out of timer
          inTimer1 = true;
          passes1 = 1;
          meters1 = 0;
          startTime1 = getTime();
        } else {
          // if we are in timer
          passes1++;
          Serial.print(F("Pss1: "));
          Serial.println(passes1);
        }
      }
      break;
    case 2: {
        lastHallWorked2 = millis();
        if (inTimer2 == false) {    //if we are out of timer
          inTimer2 = true;
          passes2 = 1;
          meters2 = 0;
          startTime2 = getTime();
        } else {
          // if we are in timer
          passes2++;
          Serial.print(F("Pss2: "));
          Serial.println(passes2);
        }
      }
      break;
      case 3: {
        lastHallWorked3 = millis();
        if (inTimer3 == false) {    //if we are out of timer
          inTimer3 = true;
          passes3 = 1;
          meters3 = 0;
          startTime3 = getTime();
        } else {
          // if we are in timer
          passes3++;
          Serial.print(F("Pss3: "));
          Serial.println(passes3);
        }
      }
      break;
    case 4: {
        lastHallWorked4 = millis();
        if (inTimer4 == false) {    //if we are out of timer
          inTimer4 = true;
          passes4 = 1;
          meters4 = 0;
          startTime4 = getTime();
        } else {
          // if we are in timer
          passes4++;
          Serial.print(F("Pss4: "));
          Serial.println(passes4);
        }
      }
      break;
      case 5: {
        lastHallWorked5 = millis();
        if (inTimer5 == false) {    //if we are out of timer
          inTimer5 = true;
          passes5 = 1;
          meters5 = 0;
          startTime5 = getTime();
        } else {
          // if we are in timer
          passes5++;
          Serial.print(F("Pss5: "));
          Serial.println(passes5);
        }
      }
      break;
    default: {
      }
      break;
  }
  digitalWrite(ledPin, LOW);
}

void stopPrintSession(int pltoStop) {
  id = readId();
  id++;
  writeId(id);
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
    case 3: {
      inTimer3 = false;
      stopTime3 = getTime();
      meters3 = passes3 / float(passesPerMeter);
      if (passes3 > 1) {
        sendDB(id, pltoStop, startTime3, stopTime3, passes3, meters3);
      }
      startTime3 = "";
      stopTime3 = "";
    }
    break;
    case 4: {
        inTimer4 = false;
        stopTime4 = getTime();
        meters4 = passes4 / float(passesPerMeter);
        if (passes4 > 1) {
          sendDB(id, pltoStop, startTime4, stopTime4, passes4, meters4);
        }
        startTime4 = "";
        stopTime4 = "";
      }
      break;
      case 5: {
        inTimer5 = false;
        stopTime5 = getTime();
        meters5 = passes5 / float(passesPerMeter);
        if (passes5 > 1) {
          sendDB(id, pltoStop, startTime5, stopTime5, passes5, meters5);
        }
        startTime5 = "";
        stopTime5 = "";
      }
      break;
  }
  Serial.println(freeRam());
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

void intHall1(){
  isHall1 = true;
}

void intHall2(){
  isHall2 = true;
}

void intHall3(){
  isHall3 = true;
}

void intHall4(){
  isHall4 = true;
}

void intHall5(){
  isHall5 = true;
}

int freeRam () {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}


