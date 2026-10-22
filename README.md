Working on it
9/30/2026

This is a monitor for the NCE Cab Bus.

Developed with an Arduino Uno R4. 
  I designed this on an Uno R4. Using Arduino built in SoftwareSerial Library
  Also tested with a Nano.

I used an RS485 board that contains a MAX 485 chip. You could easily use the IC and build your own interface
  the MAX 485 Converts Cab Bus RS485 to RS232 that is input to Arduino.
  
I used an OLED display for the status, but it is too slow to display the continuous data stream.
  I am going to attempt to display the filtered data on the OLED so that you could use the Monitor without being plugged in to the IDE.
  
3 control buttons
  Mode
  Change Cab Address
  Clear OLED Screen
  
IDE Serial Monitor

NCE SB3/5
  Cab Adress 0 to 10
  
2 modes
  Stream
  Filtered for 1 Cab Bus Address
