/**************************************************************************
 This is an example for our Monochrome OLEDs based on SSD1306 drivers

 Pick one up today in the adafruit shop!
 ------> http://www.adafruit.com/category/63_98

 This example is for a 128x64 pixel display using I2C to communicate
 3 pins are required to interface (two I2C and one reset).

 Adafruit invests time and resources providing this open
 source code, please support Adafruit and open-source
 hardware by purchasing products from Adafruit!

 Written by Limor Fried/Ladyada for Adafruit Industries,
 with contributions from the open source community.
 BSD license, check license.txt for more information
 All text above, and the splash screen below must be
 included in any redistribution.
 **************************************************************************/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library.

#define OLED_RESET -1        // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3c  ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

char OledBuf[16];  //line buffer for oled
int i;


void oled_setup() {

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(1000);  // Pause for seconds

  // Clear the buffer
  display.clearDisplay();
  display.display();
  display.setTextColor(1);      // 1=WHITE, 0=BLACK
  printOLED(OledBuf, 1, 1, 2);  // print fist line/title
}


// print text into the display buffer and light up the display
void printOLED(char sbuf[], byte xpos, byte ypos, byte fsize) {
  // (chars and lines are defined to start at 1,1 for upper-left)
  const byte h = 8;
  const byte w = 6;

  if (xpos < 1) xpos = 1;
  if (ypos < 1) ypos = 1;

  display.setTextSize(fsize);  // font size
  //clearLineOled();
  // print the line
  display.setCursor((xpos - 1) * w * fsize, (ypos - 1) * h * fsize);
  display.print(sbuf);  // print string at xy location
  display.display();    // refresh the screen
}

// clear to end of line
void clearLineOled(byte xpos, byte ypos, byte fsize) {
  //
  const byte h = 8;
  const byte w = 6;
  int16_t x,y,len,hgt;
  x=(xpos-1)*w*fsize;
  y=(ypos-1)*h*fsize;
  len=128-(xpos-1)*w*fsize;
  hgt=h*fsize;
  // clear the line
  display.setCursor(x, y);
  display.fillRect(x, y, len, hgt, SSD1306_BLACK);
}
