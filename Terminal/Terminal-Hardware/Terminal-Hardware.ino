/**
   Terminal-Hardware.ino
   Transfers the amount of money payed to the iPad
   and receives the command to print the receipt
   Board: Arduino UNO
   */

#include "Adafruit_Thermal.h"
#include "PlanP.h"
//#include "PlanP_E.h"
#include "RU.h"
#include "RD.h"

// Here's the new syntax when using SoftwareSerial (e.g. Arduino Uno) ----
// If using hardware serial instead, comment out or remove these lines:

#include "SoftwareSerial.h"
#define TX_PIN 6 // Arduino transmit  YELLOW WIRE  labeled RX on printer
#define RX_PIN 5 // Arduino receive   GREEN WIRE   labeled TX on printer

SoftwareSerial mySerial(RX_PIN, TX_PIN); // Declare SoftwareSerial obj first
Adafruit_Thermal printer(&mySerial);     // Pass addr to printer constructor
// Then see setup() function regarding serial & printer begin() calls.

// Here's the syntax for hardware serial (e.g. Arduino Due) --------------
// Un-comment the following line if using hardware serial:

//Adafruit_Thermal printer(&Serial1);      // Or Serial2, Serial3, etc.

// -----------------------------------------------------------------------


// Settings
const int SEND_INTERVAL = 500; // 0.25s

// Pins
const int PIN_COINS = 0;
const int PIN_PRINTER = 7;

// Functionality
unsigned long lastMillis = 0;
volatile int moneyCollected;
boolean moneyChanged;
String language;
String appointmentDate;
String appointmentTime;
boolean currentlyPrinting;

void setup() {
  Serial.begin(9600);

  pinMode(8, INPUT);    // KILLSWITCH IF OUTPUT
  digitalWrite(8, LOW);

  moneyCollected = 0;
  moneyChanged = false;
  language = "";
  appointmentDate = "";
  appointmentTime = "";
  currentlyPrinting = false;

  pinMode(2, INPUT_PULLUP);
  attachInterrupt(PIN_COINS, coinInserted, RISING);

  mySerial.begin(9600);  // Initialize SoftwareSerial
  //Serial1.begin(19200); // Use this instead if using hardware serial
  printer.begin();        // Init printer (same regardless of serial type)
  printer.sleep();

}

void loop() {
  while (Serial.available() && !currentlyPrinting) {
    String receivedMessage = Serial.readStringUntil('\n');
    receivedMessage.trim();
    messageReceived(getValue(receivedMessage, ':', 0), getValue(receivedMessage, ':', 1));
  }


  if (moneyChanged && millis() - lastMillis > SEND_INTERVAL && !currentlyPrinting) {
    lastMillis = millis();
    moneyChanged = false;
    Serial.println(String(moneyCollected));
  }
}

void coinInserted() {
  if (!currentlyPrinting) {
    moneyCollected = moneyCollected + 10;
    moneyChanged = true;
  }
  
}

void messageReceived(String topic, String payload) {
  // Serial.println("incoming: " + topic + " - " + payload);

  if (topic == "/language") {
    language = payload;
  } else if (topic == "/appointmentDate") {
    appointmentDate = payload;
  } else if (topic == "/appointmentTime") {
    appointmentTime = payload;
  } else if (topic == "/print") {
    printReceipt();
  } else if (topic == "/resettvm") {
    // resetTVM();
  }

}

void printReceipt() {
  currentlyPrinting = true;
  int appointmentEnding = appointmentTime != "" ? appointmentTime.toInt() + 1 : 0;
  String appointmentEndingString = appointmentEnding <= 9 ? "0" + String(appointmentEnding) : String(appointmentEnding);

  printer.wake();       // MUST wake() before printing again, even if reset
  printer.setDefault(); // Restore printer to defaults


  if (language == "de") {
    //Deutsch ***********************************************************************************
    //R up
    printer.justify('C');
    printer.printBitmap(RU_width, RU_height, RU_data);
    
    printer.justify('C');
    printer.println(("\n \n"));

    //Raum der Konfrontation
    printer.boldOn();
    printer.setSize('M');
    printer.justify('C');
    printer.println(("\n "));
    printer.println(("\nRaum der Konfrontation"));
    printer.boldOff();

    //Ticket berechtigt zum Benuetzen des Raum waehrend diesem Zeitfenster:
    printer.setSize('S');
    printer.justify('C');
    printer.println(("\n \nTicket berechtigt zum\nBenuetzung des Raum waehrend \ndieses Zeitraums:"));

    //Datum und Zeit
    printer.boldOn();
    printer.setSize('M');
    printer.justify('C');
    printer.println("\nDatum: " + appointmentDate + ".Juni.2018\nZeit: " + appointmentTime + ":00 - " + appointmentEndingString + ":00\nOrt: 5.T04 (ZHdK/Turm)");
    printer.boldOff();

    //Der Raum wird 10 Minuten vor Beginn geöffnet. 10 Minuten nach Schluss erscheint das Putzpersonal.
    printer.setSize('S');
    printer.justify('C');
    printer.println(("\nDer Raum wird 10 Minuten vor \nBeginn geoeffnet. \n10 Minuten nach Ende \nerscheint das Putzpersonal.\n \n"));

    //Plan Weg Raum
    printer.justify('C');
    printer.printBitmap(PlanP_width, PlanP_height, PlanP_data);

    //Webseite
    printer.justify('C');
    printer.println(("\n \nwww.raumderkonfrontation.com\n\n"));

    //R down
    printer.justify('C');
    printer.printBitmap(RD_width, RD_height, RD_data);

    printer.justify('C');
    printer.println(("\n\n\n\n\n\n"));
  }


  else {
    //English ***********************************************************************************
    //R up
    printer.justify('C');
    printer.printBitmap(RU_width, RU_height, RU_data);

    printer.justify('C');
    printer.println(("\n \n"));

    //Raum der Konfrontation
    printer.boldOn();
    printer.setSize('S');
    printer.justify('C');
    printer.println(("\n "));
    printer.println(("\nRoom of Confrontation"));
    printer.boldOff();

    //Ticket berechtigt zum Benuetzen des Raum waehrend diesem Zeitfenster:
    printer.setSize('S');
    printer.justify('C');
    printer.println(("\n \nTicket is valid to\nuse the room during \nthis period:"));

    //Datum und Zeit
    printer.boldOn();
    printer.setSize('M');
    printer.justify('C');
    printer.println("\nDate: " + appointmentDate + ".June.2018\nTime: " + appointmentTime + ":00 - " + appointmentEndingString + ":00\nOrt: 5.T04 (ZHdK/Tower)");
    printer.boldOff();

    //Der Raum wird 10 Minuten vor Beginn geöffnet. 10 Minuten nach Schluss erscheint das Putzpersonal.
    printer.setSize('S');
    printer.justify('C');
    printer.println(("\nThe room will be opened \n10 minutes before beginning. \n10 minutes after the end \nthe cleaning staff will arive.\n \n"));

    //Plan Weg Raum
    printer.justify('C');
    printer.printBitmap(PlanP_width, PlanP_height, PlanP_data);

    //Webseite
    printer.justify('C');
    printer.println(("\n \nwww.raumderkonfrontation.com\n\n"));

    //R down
    printer.justify('C');
    printer.printBitmap(RD_width, RD_height, RD_data);

    printer.justify('C');
    printer.println(("\n\n\n\n\n\n"));

    // ***********************************************************************************
  }


  printer.feed(4);//default is 2
  printer.setDefault(); // Restore printer to defaults
  printer.sleep();      // Tell printer to sleep
  //delay(3000L);         // Sleep for 3 seconds
  // printer.wake();       // MUST wake() before printing again, even if reset
  
  resetTVM();
}

void resetTVM() {
  moneyCollected = 0;
  appointmentDate = "";
  appointmentTime = "";
  currentlyPrinting = false;
  // pinMode(8, OUTPUT);   // lights out. Assuming it is jumper-ed correctly.
  // while(1);
}

// by H. Pauwelyn
// https://arduino.stackexchange.com/a/1237
String getValue(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
