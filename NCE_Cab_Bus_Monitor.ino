/*
basic Monitor for the NCE RS 485 cab bus
rhq 9/5/26

Start with Serial Monitor and later an Oled display,
!!! Matrrix interferes with serial stream!
!!! Don't use time stamp on serial monitor
had to modify softserial.h to be 8n2

*/

#include "SoftwareSerial.h"

// SoftwareSerial pins for listening to MAX485 RO pin
const int RS485_RX_PIN = 3;
const int RS485_TX_PIN = 5;    // Unused in listen-only mode
#define RS485_TX_ENABLE_PIN 4  // just ground the te/re pin for listen only

#define button 7
#define mode 8

byte filtercab = 0x85;  // cab address to monitor - bit 7 always set

#define ON HIGH
#define OFF LOW

SoftwareSerial CabBus(RS485_RX_PIN, RS485_TX_PIN);

char progTitle[16] = "Monitor";
extern char OledBuf[16];

byte inByte;
byte inBytestr[32];  // RS485 byte buffer

#define min_xPos 1
#define max_xPos 21
#define min_yPos 3
#define max_yPos 8
byte xPos = min_xPos;
byte yPos = min_yPos;



void setup() {

  Serial.begin(115200);
  Serial.println("NCE Cab Bus Monitor Program  9/28/26  rhq/FTT");

  sprintf(OledBuf, "%s", progTitle);      //top line on display
  oled_setup();                           // start the oled display
  sprintf(OledBuf, "%s", "Push Botton");  // instructions
  printOLED(OledBuf, xPos, yPos, 1);      // print it

  sprintf(OledBuf, "%02X", (filtercab & 0x7f));  // cab number to monitor
  printOLED(OledBuf, 9, 1, 2);

  pinMode(RS485_TX_ENABLE_PIN, OUTPUT);    // start rs485
  digitalWrite(RS485_TX_ENABLE_PIN, LOW);  // set to listen

  pinMode(button, INPUT_PULLUP);  // button for incrementing the cab address
  pinMode(mode, INPUT_PULLUP);    // mode button - print everything or print filtered data
  pinMode(LED_BUILTIN, OUTPUT);   // led for heart beat
  digitalWrite(LED_BUILTIN, OFF);

  CabBus.begin(9600);  // NCE Cab Bus baud rate

  Serial.println();
  Serial.println("==========================================");
  Serial.println("   NCE Cab Bus Passive Monitor Started    ");
  Serial.println("==========================================");
}



void loop() {
  digitalWrite(LED_BUILTIN, OFF);
  if (digitalRead(button) == LOW)
    changecab();  // change to the next cab number
  if (digitalRead(mode) == LOW)
    stdDisplay();          // monitor everything on bus
  else filter(filtercab);  // monitor only a single cab data
}




// "standard" display
//For  SB3/5  which only scans 0x80 to 0x8A,
// display 1 full scan per serial monitor line
void stdDisplay() {
  if (CabBus.available()) {
    inByte = CabBus.read();

    if (inByte == 0x80) Serial.println();  // line break on 0x80
    printByteSerial(inByte);               // format and print the characters
  }
}




// Print byte in formatted Hex
// using the OledBuf for display
void printByteSerial(byte x) {
  sprintf(OledBuf, "%02X ", x);  // convert to string
  Serial.print(OledBuf);         // put out to screen
}




void changecab() {
  filtercab++;
  if (filtercab == 0x8b)  // 8a is last for sb3/5
    filtercab = 0x80;     // roll around loop
  if (filtercab == 0x81)
    filtercab++;  // 81 not used

  clearLineOled(9, 1, 2);  // clear the slot that displays the cab #

  sprintf(OledBuf, "%02X", (filtercab & 0x7f));  // print the cab#
  printOLED(OledBuf, 9, 1, 2);
  while (digitalRead(button) == LOW)
    ;          //wait for button release
  delay(500);  //debounce
}



//**********************************************************************8
// filter display for traffic on only 1 cab address
// and only non std response
void filter(byte cab) {
  int i, j;
  byte next = cab + 1;

  if (CabBus.available()) {  // check for first byte, else return to loop()
    inByte = CabBus.read();  // get the byte

    {
      if (cab == 0x80)
        next = 0x82;
      if (cab == 0x8A)
        next = 0x80;

      // is it the cab we are using;
      if (inByte == cab) {              // otherwise return to loop()
        digitalWrite(LED_BUILTIN, ON);  // debug
        inBytestr[0] = inByte;          // stick it in the buffer

        while (!CabBus.available())
          ;                            // wait for next byte to be available
        inBytestr[1] = CabBus.read();  // get the second byte


        // case: cab followed by cab+1
        if (inBytestr[1] == next) return;



        // Read 2 more
        while (!CabBus.available())
          ;                            // wait for byte to be available
        inBytestr[2] = CabBus.read();  // get the third byte
        while (!CabBus.available())
          ;                            // wait for byte to be available
        inBytestr[3] = CabBus.read();  // get the forth byte


        // Case: no button, no new speed
        if ((inBytestr[1] == 0x7d) && (inBytestr[2] == 0x7f) && (inBytestr[3] == next)) return;



        // case: 2 bytes sent back to Command Station
        // display bracketed by cab and cab+1
        if (inBytestr[3] == next) {
          for (i = 0; i < 4; i++) {
            printByteSerial(inBytestr[i]);
          }
          Serial.println();
          return;
        }



        // case: long command to cab, usually chars for diaplay
        // read until cab+1, while filling buffer
        i = 3;
        inBytestr[4] = 0;  // init the byte

        while (inBytestr[i] != next && i < 30) {
          while (!CabBus.available())
            ;                              // wait for byte to be available
          inBytestr[++i] = CabBus.read();  // get the next byte
        }

        for (j = 0; j <= i; j++) {
          printByteSerial(inBytestr[j]);
        }
        Serial.println();
        return;
      }
    }
  }
}



///******************************************************
// oled display too slow 9600 baud data stream
void printByteOled() {

  if (inByte == 0x80 | inByte == 0x84 | inByte == 0x87) {
    yPos++;                                // next line
    if (yPos > max_yPos) yPos = min_yPos;  // back to the top
    xPos = min_xPos;                       // cr
    clearLineOled(xPos, yPos, 1);
  }
  sprintf(OledBuf, "%02X ", inByte);      //stick it in the buffer
  printOLED(OledBuf, xPos, yPos, 1);      // print it
  xPos += 3;                              // move over
  if (xPos >= max_xPos) xPos = min_xPos;  // cr
}
