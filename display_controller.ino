
#include "UDI_RC_DISPLAY.h"

UDI_RC_DISPLAY lcd; // create an "lcd" object


void setup(){
  // start the lcd:
  // cs to pin 13
  // wr to pin 12
  // Data to pin 8
  // backlight to pin 10
  // you can chose whichever pin you want


  Serial.begin(9600);
  lcd.begin(13, 12, 8, 10); // (cs, wr, Data, backlight)
  // if no backlight control is given, you can also use:
  // lcd.begin(13, 12, 8); // (cs, wr, Data)

  lcd.clear(); // clear the screen

  lcd.initDisp();
  delay(1000);
}

void loop() {

  for (int i = 0; i < 101; i++)
  {
    lcd.writePercentage(i);
    lcd.update();
    delay(250);
  }
  /*for (int i = 0; i < 8; i++)
  {
    //Serial.println(1 << i);
    lcd._buffer[4] = 1 << i;
    lcd.update();
    delay(1000);
  }*/
}
