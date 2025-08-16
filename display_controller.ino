#include <SPI.h>
#include <RF22.h>
#include <UDI_RC_DISPLAY.h>
#include <Adafruit_Keypad.h>

/*
 * Define pins for the keypad matrix
 */
#define R1    5
#define R2    6
#define R3    7
#define R4    9
#define C1    3
#define C2    4
#define C3    17

/*
 * Define analog inputs for pots
 */
#define throttlePin     A7
#define anglePin        A6
#define tailPin         A5
#define ailronPin       A4

/*
 * Pots sensibility not spam messaging service
 */
#define pot_sensibility   1

/*
 * Values read from Pots
 */
int throttleValue   = 0;
int angleValue      = 0;
int tailValue       = 0;
int ailronValue     = 0;
/*
 * Previous Pots values to check with current values
 */
int last_throttleValue   = 0;
int last_angleValue      = 0;
int last_tailValue       = 0;
int last_ailronValue     = 0;

/* 
 * Singleton instance of the radio 
 */
RF22 rf22;

/* 
 * create an "lcd" object 
 */
UDI_RC_DISPLAY lcd;

/* 
 * create an "keypad" object 
 */
Adafruit_Keypad customKeypad = Adafruit_Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS);


/* 
 * "keypad" params
 */
const byte ROWS = 4;                        // rows
const byte COLS = 3;                        // columns
char keys[ROWS][COLS] = { {'L', '3', '6'},  // define the symbols on the buttons of the keypads
                          {'R', '2', '5'}, 
                          {'7', '9', '1'}, 
                          {'8', '0', '4'}};
byte rowPins[ROWS] = {R1, R2, R3, R4};      // connect to the row pinouts of the keypad
byte colPins[COLS] = {C1, C2, C3};          // connect to the column pinouts of the keypad



void setup(){
  // init serial comm
  Serial.begin(9600);

  // init RF22
  if (!rf22.init()) {
    Serial.println("RF22 init failed");
    // Defaults after init are 434.0MHz, 0.05MHz AFC pull-in, modulation FSK_Rb2_4Fd36  
  }
  else {
    Serial.println("RF22 init succeeded");
  }

  // init keypad
  customKeypad.begin();

  // init LCD Display
  lcd.begin(13, 12, 8);
  lcd.init();
  delay(100);
}

void loop() {

  readPots();

  // put your main code here, to run repeatedly:
  customKeypad.tick();

  while(customKeypad.available()){
    keypadEvent e = customKeypad.read();
    Serial.print((char)e.bit.KEY);
    if(e.bit.EVENT == KEY_JUST_PRESSED) Serial.println(" pressed");
    else if(e.bit.EVENT == KEY_JUST_RELEASED) Serial.println(" released");
  }

  delay(10);

  // testDisplay();
}

void readPots() {
  throttleValue   = analogRead(throttlePin);
  angleValue      = analogRead(anglePin);
  tailValue       = analogRead(tailPin);
  ailronValue     = analogRead(ailronPin);

  if (((throttleValue > last_throttleValue + pot_sensibility) || (throttleValue < last_throttleValue - pot_sensibility))
    || ((angleValue > last_angleValue + pot_sensibility)      || (angleValue < last_angleValue - pot_sensibility))
    || ((tailValue > last_tailValue + pot_sensibility)        || (tailValue < last_tailValue - pot_sensibility))
    || ((ailronValue > last_ailronValue + pot_sensibility)    || (ailronValue < last_ailronValue - pot_sensibility))) {
    Serial.print(throttleValue);
    Serial.print(" : ");
    Serial.print(angleValue);
    Serial.print(" : ");
    Serial.print(tailValue);
    Serial.print(" : ");
    Serial.println(ailronValue);

    last_throttleValue = throttleValue;
    last_angleValue = angleValue;
    last_tailValue = tailValue;
    last_ailronValue = ailronValue;
  }
}

void testDisplay() {
  // test light
  lcd.setLight(false);
  lcd.update();
  delay(1000); 
  lcd.setLight(true);
  lcd.update();
  delay(1000); 

  // test mode
  lcd.setMode(mode_mode1);
  lcd.update();
  delay(1000); 
  lcd.setMode(mode_mode2);
  lcd.update();
  delay(1000);  
  lcd.setMode(mode_none);
  lcd.update();
  delay(1000);  

  // test camera mode
  lcd.setCameraMode(camera_photo);
  lcd.update();
  delay(1000);  
  lcd.setCameraMode(camera_video);
  lcd.update();
  delay(1000);
  lcd.setCameraMode(camera_none);
  lcd.update();
  delay(1000);
  
  // test Network level
  for (int i = 0; i < 6; i++)
  {
    lcd.setNetworkLevel(i);
    lcd.update();
    delay(500);
  }

  // test throttle select
  lcd.setThrottleType(throttle_thro);
  lcd.update();
  delay(1000);
  lcd.setThrottleType(throttle_trim);
  lcd.update();
  delay(1000);
  lcd.setThrottleType(throttle_DR);
  lcd.update();
  delay(1000);
  lcd.setThrottleType(throttle_none);
  lcd.update();
  delay(1000);

  // test battery level
  for (int i = 0; i < 6; i++)
  {
    lcd.setBatteryLevel(i);
    lcd.update();
    delay(250);
  }

  // test percentage write
  for (int i = 0; i < 101; i++)
  {
    lcd.writePercentage(i);
    lcd.update();
    delay(250);
  }
}

void stopForever() {
  while (1) {    
  }
}
