#include <Arduino.h>
#include "UDI_RC_DISPLAY.h"

UDI_RC_DISPLAY::UDI_RC_DISPLAY(){
	_buffer[0] = 0x00;
	_buffer[1] = 0x00;
	_buffer[2] = 0x00;
	_buffer[3] = 0x00;
	_buffer[4] = 0x00;
	_buffer[5] = 0x00;
	_buffer[6] = 0x00;
}

void UDI_RC_DISPLAY::begin(int cs_p, int wr_p, int data_p, int backlight_p)
{
	pinMode(cs_p, OUTPUT);
	pinMode(wr_p, OUTPUT);
	pinMode(data_p, OUTPUT);
	pinMode(backlight_p, OUTPUT);
	_cs_p=cs_p;
	_wr_p=wr_p;
	_data_p=data_p;
	_backlight_p=backlight_p;
	_backlight_en=true;
	config();
}

void UDI_RC_DISPLAY::begin(int cs_p, int wr_p, int data_p)
{
	pinMode(cs_p, OUTPUT);
	pinMode(wr_p, OUTPUT);
	pinMode(data_p, OUTPUT);
	_cs_p=cs_p;
	_wr_p=wr_p;
	_data_p=data_p;
	_backlight_en = false;
	config();
}

void UDI_RC_DISPLAY::wrDATA(unsigned char data, unsigned char cnt) {
	unsigned char i;
	for (i = 0; i < cnt; i++) {
		digitalWrite(_wr_p, LOW);
		delayMicroseconds(4);
		if (data & 0x80) {
			digitalWrite(_data_p, HIGH);
		}
		else {
			digitalWrite(_data_p, LOW);
		}
		digitalWrite(_wr_p, HIGH);
		delayMicroseconds(4);
		data <<= 1;
	}
}
void UDI_RC_DISPLAY::wrclrdata(unsigned char addr, unsigned char sdata)
{
	addr <<= 2;
	digitalWrite(_cs_p, LOW);
	wrDATA(0xa0, 3);
	wrDATA(addr, 6);
	wrDATA(sdata, 8);
	digitalWrite(_cs_p, HIGH);
}

void UDI_RC_DISPLAY::display()
{
	wrCMD(LCDON);
}

void UDI_RC_DISPLAY::noDisplay()
{
	wrCMD(LCDOFF);
}

void UDI_RC_DISPLAY::wrone(unsigned char addr, unsigned char sdata)
{
	addr <<= 2;
	digitalWrite(_cs_p, LOW);
	wrDATA(0xa0, 3);
	wrDATA(addr, 6);
	wrDATA(sdata, 8);
	digitalWrite(_cs_p, HIGH);
}

void UDI_RC_DISPLAY::wrCMD(unsigned char CMD) {  //100
	digitalWrite(_cs_p, LOW);
	wrDATA(0x80, 4);
	wrDATA(CMD, 8);
	digitalWrite(_cs_p, HIGH);
}

void UDI_RC_DISPLAY::config()
{
	wrCMD(BIAS);
	wrCMD(RC256);
	wrCMD(SYSDIS);
	wrCMD(WDTDIS1);
	wrCMD(SYSEN);
	wrCMD(LCDON);
}

void UDI_RC_DISPLAY::wrCLR(unsigned char len) {
	unsigned char addr = 0;
	unsigned char i;
	for (i = 0; i < len; i++) {
		wrclrdata(addr, 0x00);
		addr = addr + 2;
	}
}

void UDI_RC_DISPLAY::setBatteryLevel(int level) {
	// zero out the previous (otherwise the or couldn't be possible)
	_buffer[0] &= 0x7F;
	_buffer[1] &= 0x7F;
	_buffer[2] &= 0x7F;

	switch(level){
		case 3: // battery on and all 3 segments
			_buffer[0] |= 0x80;
		case 2: // battery on and 2 segments
			_buffer[1] |= 0x80;
		case 1: // battery on and 1 segment
			_buffer[2] |= 0x80;
		case 0: // battery indication off
		default:
			break;
	}

	update();
}

void UDI_RC_DISPLAY::clear(){
	wrCLR(16);
}

// takes the buffer and puts it straight into the driver
void UDI_RC_DISPLAY::update(){
	// the buffer is backwards with respect to the lcd. could be improved
	wrone(0, _buffer[5]);
	wrone(2, _buffer[4]);
	wrone(4, _buffer[3]);
	wrone(6, _buffer[2]);
	wrone(8, _buffer[1]);
	wrone(10,_buffer[0]);
}

char UDI_RC_DISPLAY::decTo7Seg(int dec) {
	if (dec < 0) {
		dec = 0;
	}
	else if (dec > 9) {
		dec = 9;
	}
	switch (dec) {
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

char UDI_RC_DISPLAY::charTo7Seg(int dec) {
	switch (dec) {
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

void UDI_RC_DISPLAY::writeDec(int num, int pos) {
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
  _buffer[pos + 2] &= buf2ZerosMask;
	_buffer[pos + 2] |= buf2OnesMask;

	_buffer[pos + 3] &= buf3ZerosMask;
	_buffer[pos + 3] |= buf3OnesMask;
}

void UDI_RC_DISPLAY::writeCent(int cent) {
	if (cent == 0) {
		_buffer[2] &= 0xEF;
	}
	else {
		_buffer[2] |= 0x10;
	}
}

void UDI_RC_DISPLAY::writePercentage(int percent) {
	int unit = 0, dec = 0, cent = 0;

	cent = percent / 100;
	dec = (percent % 100) / 10;
	unit = percent % 10;

	writeCent(cent);
	writeDec(dec, 0);
	writeDec(unit, 1);
}

void UDI_RC_DISPLAY::initDisp() {
  // activate Battery
  _buffer[0] |= 0x08;

  // activate Battery
  _buffer[1] |= 0x04;

  // activate Throttle
  _buffer[2] |= 0x04;
  
  // activate Network
  _buffer[5] |= 0x8F;

  // animate to 100% and back to 0%
  for (int i = 0; i < 100; i++) {
    writePercentage(i);
    update();
    delay(10);
  }

  writePercentage(100);
  update();
  delay(400);

  for (int i = 100; i >= 0; i--) {
    writePercentage(i);
    update();
    delay(10);
  }

  writePercentage(0);
  update();
}

void UDI_RC_DISPLAY::writePercentage(int percent) {

}

void UDI_RC_DISPLAY::setThrottleType(ThrottleType type) {

}

void UDI_RC_DISPLAY::setNetworkLevel(unsigned char level) {

}

void UDI_RC_DISPLAY::setBatteryLevel(unsigned char level) {

}

void UDI_RC_DISPLAY::setCameraMode(CameraType camera) {

}

void UDI_RC_DISPLAY::setLight(bool state) {

}

void UDI_RC_DISPLAY::setMode(ModeType mode) {

}
