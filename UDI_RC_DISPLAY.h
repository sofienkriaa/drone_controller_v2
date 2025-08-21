#ifndef UDI_RC_DISPLAY_H_
#define UDI_RC_DISPLAY_H_

#define  BIAS     0x52             //0b1000 0101 0010  1/3duty 4com
#define  SYSDIS   0X00             //0b1000 0000 0000  关振系统荡器和LCD偏压发生器
#define  SYSEN    0X02             //0b1000 0000 0010 打开系统振荡器
#define  LCDOFF   0X04             //0b1000 0000 0100  关LCD偏压
#define  LCDON    0X06             //0b1000 0000 0110  打开LCD偏压
#define  XTAL     0x28             //0b1000 0010 1000 外部接时钟
#define  RC256    0X30             //0b1000 0011 0000  内部时钟
#define  TONEON   0X12             //0b1000 0001 0010  打开声音输出
#define  TONEOFF  0X10             //0b1000 0001 0000 关闭声音输出
#define  WDTDIS1  0X0A             //0b1000 0000 1010  禁止看门狗
#define  BUFFERSIZE 12

typedef enum ThrottleType
{
	throttle_thro = 0,
	throttle_trim = 1,
	throttle_DR = 2,
	throttle_none = 99
} ThrottleType;

typedef enum CameraType
{
	camera_photo = 0,
	camera_video = 1,
	camera_none = 99
} CameraType;

typedef enum ModeType
{
	mode_mode1 = 0,
	mode_mode2 = 1,
	mode_none = 99
} ModeType;

/*
* choose to print Integer or Char
*/
typedef enum PrintType
{
	print_int = 0,
	print_char = 1
} PrintType;

/*
* Error type
*/
typedef enum ErrorType
{
	rfNotFound_err			= 0,
	receiverNotFound_err	= 1
} ErrorType;

class  UDI_RC_DISPLAY
{
public:
	UDI_RC_DISPLAY();
	void begin(int cs_p, int wr_p, int data_p, int backlight_p);
	void begin(int cs_p, int wr_p, int data_p);
	void init();
	void clear();	
	void update();
	
	void writePercentage(int percent);
	void setThrottleType(ThrottleType type);
	void setNetworkLevel(unsigned char level);
	void setBatteryLevel(int level);
	void setCameraMode(CameraType camera);
	void setLight(bool state);
	void setMode(ModeType mode);

	void printError(ErrorType errorCode);
	void writeDec(char num, int pos, PrintType pType);
private:
	int _cs_p;
	int _wr_p;
	int _data_p;
	int _backlight_p;
	bool _backlight_en;
	char _buffer[BUFFERSIZE];
	unsigned char _battery[3];
	void wrone(unsigned char addr, unsigned char sdata);
	void wrclrdata(unsigned char addr, unsigned char sdata);
	void wrCLR(unsigned char len);
	void wrDATA(unsigned char data, unsigned char cnt);
	void wrCMD(unsigned char CMD);
	void config(); // legacy: why not in begin func
	char binTo7Seg(char dec, PrintType pType);
	void writeCent(int cent);

};
#endif
