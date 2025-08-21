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
	// activate frame
	_buffer[1] |= 0x04;
	// reset all to zeros
	_buffer[1] &= 0B10001100;

	switch(level){
		case 5:
			_buffer[1] |= 0x73;
			break;
		case 4:
			_buffer[1] |= 0x72;
			break;
		case 3:
			_buffer[1] |= 0x70;
			break;
		case 2:
			_buffer[1] |= 0x30;
			break;
		case 1:
			_buffer[1] |= 0x10;
			break;
		case 0:
		default:
			break;
	}
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

char UDI_RC_DISPLAY::binTo7Seg(char dec, PrintType pType) {

	if (pType == print_int) {
		if (dec < 0) {
			dec = 0;
		}
		else if (dec > 9) {
			dec = 9;
		}
		switch (dec) {
			case 0:
				return 0x7D;
				break;
			case 1:
				return 0x30;
				break;
			case 2:
				return 0x6B;
				break;
			case 3:
				return 0x7A;
				break;
			case 4:
				return 0x36;
				break;
			case 5:
				return 0x5E;
				break;
			case 6:
				return 0x5F;
				break;
			case 7:
				return 0x70;
				break;
			case 8:
				return 0x7F;
				break;
			case 9:
				return 0x7E;
				break;
			default:
				break;
		}
	}
	else if (pType == print_char) {
		switch (dec) {
			case 'A':
				return 0x77;
				break;
			case 'B':
				return 0x1F;
				break;
			case 'C':
				return 0x4D;
				break;
			case 'D':
				return 0x2B;
				break;
			case 'E':
				return 0x4F;
				break;
			case 'F':
				return 0x47;
				break;
			case 'G':
				return 0x7E;
				break;
			case 'H':
				return 0x37;
				break;
			case 'I':
				return 0x30;
				break;
			case 'J':
				return 0x38;
				break;
			case 'K':
				return 0x57;
				break;
			case 'L':
				return 0x0D;
				break;
			case 'M':
				return 0x75;
				break;
			case 'N':
				return 0x13;
				break;
			case 'O':
				return 0x33;
				break;
			case 'P':
				return 0x67;
				break;
			case 'Q':
				return 0x6F;
				break;
			case 'R':
				return 0x03;
				break;
			case 'S':
				return 0x5E;
				break;
			case 'T':
				return 0x0F;
				break;
			case 'U':
				return 0x19;
				break;
			case 'V':
				return 0x3D;
				break;
			case 'W':
				return 0x3D;
				break;
			case 'X':
				return 0x37;
				break;
			case 'Y':
				return 0x3E;
				break;
			case 'Z':
				return 0x6B;
				break;
			default:
				break;
		}
	}
}

void UDI_RC_DISPLAY::writeDec(char num, int pos, PrintType pType) {
	// pos 0 --> units
	// pos 1 --> decimals

	byte bin = binTo7Seg(num, pType);

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
	writeDec(dec, 0, print_int);
	writeDec(unit, 1, print_int);
}

void UDI_RC_DISPLAY::init() {
	// activate light
	setLight(false);

	// activate Battery
	setBatteryLevel(0);
  
	// activate Throttle
	setThrottleType(throttle_none);
  
	// activate Network
	setNetworkLevel(0);

	// activate camera
	setCameraMode(camera_none);

	// activate mode
	setMode(mode_none);

	// activate parenthesis
	_buffer[4] |= 0xC0;
	_buffer[0] |= 0x80;
	_buffer[1] |= 0x80;

	// activate points
	_buffer[5] |= 0x0F;

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

	// print 00 at the start
	writePercentage(0);
	update();
}

void UDI_RC_DISPLAY::setThrottleType(ThrottleType type) {
	// reset all to zeros
	_buffer[2] &= 0xF8;

	if (type == throttle_thro) {
		_buffer[2] |= 0x04;
	}
	else if (type == throttle_trim) {
		_buffer[2] |= 0x02;
	}
	else if (type == throttle_DR) {
		_buffer[2] |= 0x01;
	}
}

void UDI_RC_DISPLAY::setNetworkLevel(unsigned char level) {

	// Activate logo
	_buffer[5] |= 0x80;
	// reset all to zeros
	_buffer[0] &= 0xF8;
	_buffer[4] &= 0xDF;
	_buffer[5] &= 0xBF;

	switch (level) {
	case 5:
		_buffer[0] |= 0x07;
		_buffer[4] |= 0x20;
		_buffer[5] |= 0x40;
		break;
	case 4:
		_buffer[0] |= 0x06;
		_buffer[4] |= 0x20;
		_buffer[5] |= 0x40;
		break;
	case 3:
		_buffer[0] |= 0x04;
		_buffer[4] |= 0x20;
		_buffer[5] |= 0x40;
		break;
	case 2:
		_buffer[4] |= 0x20;
		_buffer[5] |= 0x40;
		break;
	case 1:
		_buffer[5] |= 0x40;
		break;
	case 0:
	default:
		break;
	}
}

void UDI_RC_DISPLAY::setCameraMode(CameraType camera) {
	// reset all to zeros
	_buffer[1] &= 0xF7;
	_buffer[2] &= 0xF7;

	if (camera == camera_photo) {
		_buffer[1] |= 0x08;
	}
	else if (camera == camera_video) {
		_buffer[2] |= 0x08;
	}
}

void UDI_RC_DISPLAY::setLight(bool state) {
	// reset all to zeros
	_buffer[0] &= 0xF7;

	if (state == true) {
		_buffer[0] |= 0x08;
	}
}

void UDI_RC_DISPLAY::setMode(ModeType mode) {
	// reset all to zeros
	_buffer[5] &= 0xCF;

	if (mode == mode_mode1) {
		_buffer[5] |= 0x10;
	}
	else if (mode == mode_mode2) {
		_buffer[5] |= 0x20;
	}
}

void UDI_RC_DISPLAY::printError(ErrorType errorCode) {

	while (true) {
		// print Er for 1 sec
		writeDec('E', 0, print_char);
		writeDec('R', 1, print_char);
		update();

		delay(1000);

		// print Error code for 1 sec
		writePercentage(1);
		update();

		delay(1000);
	}
}