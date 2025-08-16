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
#define pot_sensibility       1

/*
 * RF22 handshake tries number
 */
#define HANDSHAKE_TRY_CYCLES  3

/*
 * Handshake strings
 */
#define HAND_SHAKE_TRANSMIT "Hello!"
#define HAND_SHAKE_RECEIVES "Hello Back!"

/*
 * handshake return type
 */
typedef enum Handshake_ret
{
  handshake_success = 0,
  handshake_no_receiver = 1,
  handshake_no_msg = 2
} Handshake_ret;

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
 * transmit variable
 */
bool isTransmit = false;
uint8_t transmitVal_1 = 0;
uint8_t transmitVal_2 = 0;

/* 
 * Singleton instance of the radio 
 */
RF22 rf22;

/* 
 * create an "lcd" object 
 */
UDI_RC_DISPLAY lcd;

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
/* 
 * create an "keypad" object 
 */
Adafruit_Keypad customKeypad = Adafruit_Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS);


void setup(){
  // init serial comm
  Serial.begin(9600);

  // init LCD Display
  lcd.begin(13, 12, 8);
  lcd.init();

  // init RF22
  if (!rf22.init()) {
    // Defaults after init are 434.0MHz, 0.05MHz AFC pull-in, modulation FSK_Rb2_4Fd36  
    Serial.println("RF22 init failed");

    // TODO: Add Error Message in LCD
    
    // could not continue without RF transmitter
    stopForever();
  }
  else {
    // RF22 successfully initialized
    Serial.println("RF22 init succeeded");

    // Handshake with the RF22
    Handshake_ret handshakeState = rf22Handshake();

    if (handshakeState != handshake_success) {
      // could not have a successful handshake
      stopForever();
    }
  }

  // init keypad
  customKeypad.begin();

  delay(100);
}

void loop() {
  // read and save Pots values
  isTransmit = readPots();

  // read buttons
  customKeypad.tick();

  while(customKeypad.available()){
    keypadEvent e = customKeypad.read();
    char pressedKey = (char)e.bit.KEY;
    Serial.print(pressedKey);
    if(e.bit.EVENT == KEY_JUST_PRESSED) {
      Serial.println(" pressed");
      registerCommand(pressedKey);
      isTransmit = true;
    }
    // TODO: else if(e.bit.EVENT == KEY_JUST_RELEASED) Serial.println(" released");
  }

  if (isTransmit == true){
    // TODO: rf22.send(data, sizeof(data));
  }
  
  delay(10);

  // TODO: testDisplay();
}

bool readPots() {
  // is transmit needed
  bool isTransmNeeded = false;
  
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

    isTransmNeeded = false;
  }

  return isTransmNeeded;
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

void registerCommand(char pressedKey) {
  switch(pressedKey){
    case 'L':
      transmitVal_1 |= 0x01;
      break;
    case '3':
      transmitVal_1 |= 0x02;
      break;
    case '6':
      transmitVal_1 |= 0x04;
      break;
    case 'R':
      transmitVal_1 |= 0x08;
      break;
    case '2':
      transmitVal_1 |= 0x10;
      break;
    case '5':
      transmitVal_1 |= 0x20;
      break;
    case '7':
      transmitVal_1 |= 0x40;
      break;
    case '9':
      transmitVal_1 |= 0x80;
      break;
    case '1':
      transmitVal_2 |= 0x01;
      break;
    case '8':
      transmitVal_2 |= 0x02;
      break;
    case '0':
      transmitVal_2 |= 0x04;
      break;
    case '4':
      transmitVal_2 |= 0x08;
      break;
    default:
      break;
  }
}

Handshake_ret rf22Handshake() {
  // return value
  Handshake_ret milkShake = handshake_no_receiver;

  // hand shake string
  uint8_t data[] = HAND_SHAKE_TRANSMIT;
  rf22.send(data, sizeof(data));
  
  rf22.waitPacketSent();
  // Now wait for a reply
  char buf[RF22_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);

  // repeat handshake multiple times
  for (int i = 0; i < HANDSHAKE_TRY_CYCLES; i++) {
    if (rf22.waitAvailableTimeout(500))
    { 
      // Should be a message for us now   
      if (rf22.recv((uint8_t*)buf, &len))
      {
        Serial.print("got reply: ");
        Serial.println((char*)buf);
    
        if (buf != HAND_SHAKE_RECEIVES) {
          milkShake = handshake_success;
          break;
        }
      }
      else
      {
        Serial.println("recv failed");
        milkShake = handshake_no_msg;
      }
    }
    else
    {
      Serial.println("No reply, is rf22_server running?");
    }
  }

  return milkShake;
}

void stopForever() {
  while (1) {
  }
}
