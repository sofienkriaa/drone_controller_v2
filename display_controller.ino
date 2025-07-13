/*
  Hello World!
  Displays the milliseconds passed since power on.
  Using an HT1621 based 7-segment LCD with 6 digits
  The circuit:
  cs to pin 13
  wr to pin 12
  Data to pin 8
  backlight to pin 10
  Created 9 dec 2018
  By valerio\new (5N44P)

  https://github.com/5N44P/ht1621-7-seg
*/

#include <HT1621.h> // include our library


HT1621 lcd; // create an "lcd" object


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

  lcd.backlight(); // turn on the backlight led

  lcd.clear(); // clear the screen
}

void loop(){

  //Serial.println("0");
  lcd._buffer[5] = 0x00;
  lcd._buffer[4] = 0x00;
  lcd._buffer[3] = 0x00;
  lcd._buffer[2] = 0x00;
  lcd._buffer[1] = 0x00;
  lcd._buffer[0] = 0x00;

  lcd.update();
  delay(1000);

  /*lcd._buffer[2] = 0b00010000;
  lcd._buffer[3] = 0b00001110;
  lcd._buffer[4] = 0b00001110;
  lcd.update();
  delay(1000);*/

  for (int i = 0; i < 101; i++)
  {
    writePercentage(i);
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

void writePercentage(int percent)
{
  int unit = 0, dec = 0, cent = 0;

  cent = percent / 100;
  dec = (percent % 100) / 10;
  unit = percent % 10;

  writeCent(cent);
  writeDec(dec, 0);
  writeDec(unit, 1);
}

void writeCent(int cent)
{
  if (cent == 0)
  {
    lcd._buffer[2] &= 0xEF;
  }
  else
  {
    lcd._buffer[2] |= 0x10;
  }
}

void writeDec(int num, int pos)
{
  // pos 0 --> units
  // pos 1 --> decimals
  
  byte bin = decTo7Seg(num);

  // create masks
  // 3 first bits in Buffer 2, the rest for Buffer 3
  byte buf2ZerosMask = (bin << 5) | 0x1F;
  byte buf2OnesMask = (bin << 5) & 0xE0;

  byte buf3ZerosMask = (bin >> 3) | 0xE0;
  byte buf3OnesMask = (bin >> 3) & 0x1F;

  // Apply on Buffer 2 and 3  
  lcd._buffer[pos + 2] &= buf2ZerosMask;
  lcd._buffer[pos + 2] |= buf2OnesMask;

  lcd._buffer[pos + 3] &= buf3ZerosMask;
  lcd._buffer[pos + 3] |= buf3OnesMask;
}

byte decTo7Seg(int dec) {
  if (dec < 0) {
    dec = 0;
  }
  else if (dec > 9) {
    dec = 9;
  }
  switch(dec) {
    case 0:
      return 0x7D;
    case 1:
      return 0x30;
    case 2:
      return 0x6B;
    case 3:
      return 0x7A;
    case 4:
      return 0x36;
    case 5:
      return 0x5E;
    case 6:
      return 0x5F;
    case 7:
      return 0x70;
    case 8:
      return 0x7F;
    case 9:
      return 0x7E;
  }
}

byte charTo7Seg(int dec) {
  switch(dec) {
    case 'A':
      return 0x00;
    case 'a':
      return 0x00;
    case 'B':
      return 0x00;
    case 'b':
      return 0x00;
    case 'C':
      return 0x00;
    case 'c':
      return 0x00;
    case 'D':
      return 0x00;
    case 'd':
      return 0x00;
    case 'E':
      return 0x00;
    case 'e':
      return 0x00;
  }
}
