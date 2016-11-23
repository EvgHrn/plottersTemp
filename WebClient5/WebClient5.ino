
#include <Wire.h>
#include "RTClib.h"
#include <SPI.h>
#include <Ethernet.h>
#include "Arduino.h"

#define maxPassDelay  8000
#define passesPerMeter 80
#define ledPin 5
#define int5Pin 3

byte mac[] = { 0xDA, 0xAE, 0xBE, 0xEF, 0xFE, 0xED };

IPAddress server(185, 154, 12, 69); // numeric IP for Google (no DNS)

// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(192, 168, 0, 177);

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client;

RTC_DS1307 rtc;

boolean inTimer5 = false;
unsigned int passes5 = 0;
float meters5 = 0.0;
unsigned long id = 0;
String startTime5 = "";
String stopTime5 = "";
unsigned long lastHallWorked5 = 0;
volatile boolean isHall5 = false;


void setup() {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  pinMode(int5Pin, INPUT_PULLUP);

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
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
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

  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  
  attachInts();

  Serial.println(F("SetupF"));
  delay(1000);
  
  interrupts();
}

void loop() {
  
  if (isHall5) {
    //digitalWrite(ledPin, HIGH);
    hall_worked(5);
    isHall5 = false;
  }

  if ((inTimer5 == true) && ((millis() - lastHallWorked5) > maxPassDelay) ) {
    stopPrintSession(5);
  }
}

void sendDB(int _id, byte _plotter, String _startTime, String _stopTime, int _passes, float _meters) {
  //Serial.println(freeRam());
  detachInts();
  String post = String("id=") + _id + String("&plotter=") + _plotter + String("&startTime=") + _startTime + String("&stopTime=") + _stopTime + String("&passes=") + _passes + String("&meters=") + _meters;
  //Serial.println(freeRam());
  if (client.connect(server, 3000)) {
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
  }
  attachInts();
}

void hall_worked(int pl) { 
  //Serial.println(freeRam());
  //Serial.print("Worked ");
  //Serial.println(pl);
  switch (pl) {
    case 1: {
      }
      break;
    case 2: {
      }
      break;
      case 3: {
      }
      break;
    case 4: {
      }
      break;
      case 5: {
        lastHallWorked5 = millis();
        if (inTimer5 == false) {    //if we are out of timer
          inTimer5 = true;
          passes5 = 1;
          meters5 = 0;
          detachInts();
          startTime5 = getTime();
          attachInts();
          Serial.print(F("Pss5: "));
          Serial.println(passes5);
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
  //id = readId();
  id++;
  //writeId(id);
  switch (pltoStop) {
    case 1: {
      }
      break;
    case 2: {
      }
      break;
    case 3: {
    }
    break;
    case 4: {
      }
      break;
      case 5: {
        inTimer5 = false;
        detachInts();
        stopTime5 = getTime();
        attachInts();
        meters5 = passes5 / float(passesPerMeter);
        if (passes5 > 1) {
          sendDB(id, pltoStop, startTime5, stopTime5, passes5, meters5);
        }
        startTime5 = "";
        stopTime5 = "";
        meters5 = 0;
        passes5 = 0;
      }
      break;
  }
  //Serial.println(freeRam());
}

String getTime() {
  DateTime now = rtc.now();
  return String(String(now.year()) + "-" + String(now.month()) + "-" + String(now.day()) + " " + String(now.hour()) + ":" + String(now.minute()));
}

void intHall5(){
  isHall5 = true;
}

int freeRam () {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

void attachInts(){
  attachInterrupt(digitalPinToInterrupt(int5Pin), intHall5, FALLING);
}

void detachInts(){
  detachInterrupt(digitalPinToInterrupt(int5Pin));
}


