#include <FastLED.h>

#define LED_PIN     14
#define NUM_LEDS    1
#define BRIGHTNESS  100
#define LED_TYPE    WS2811
#define COLOR_ORDER RGB
CRGB leds[NUM_LEDS];

int left;
byte leftD;
byte leftS;

int right;
byte rightD;
byte rightS;

byte drive;

int turret;
byte turretD;
byte turretS;

byte triggerB;

int barrel;
int fire;
int sound;
int powerUpUse;

//String toTank;
byte toTank[2] = {0,0};

int speedMult;
int fireRateMult;
int powerUp;
int destroyed;

int leftX;
int leftY;
int rightX;
int rightY;
int gunX;
int gunY;
int controllSwitch;

int gunner;
int gunRight;
int gunLeft;

long int milli = 0;

#define DATA 5
#define LATCH 6
#define CLOCK 7

#define RED 13
#define YELLOW 12

unsigned long rFlash = millis();
bool rOn = 0;

//analog pins
//for the old controller mockup
/*#define LEFT_X_PIN 4
#define LEFT_Y_PIN 5
#define RIGHT_X_PIN 2
#define RIGHT_Y_PIN 3
#define GUN_X_PIN 4
#define GUN_Y_PIN 5*/

//For the new pcb
#define RIGHT_X_PIN 0
#define RIGHT_Y_PIN 1
#define LEFT_X_PIN 2
#define LEFT_Y_PIN 3
#define GUN_X_PIN 4
#define GUN_Y_PIN 5

//digital pins
#define TRIGGER_PIN 2
#define POWEUP_PIN 3
#define GUNNER_TRIGGER_PIN 8
#define GUNNER_TOGGLE_PIN 9

//digital out pins
#define RED_PIN 4
#define GREEN_PIN 5
#define BLUE_PIN 6

#define X_DEAD_ZONE 40
#define Y_DEAD_ZONE 40
#define TURRET_DEAD_ZONE 50
#define BARREL_DEAD_ZONE 40
#define JOY_MAX 1023

void prntBits(byte b, char lnEnd='\0');

void ledColour(int, int, int);

void setup() {
  //wifi serial
  Serial.begin(9600);
  
  //trigger
  pinMode(TRIGGER_PIN, INPUT_PULLUP);
  
  //gunner toggle
  //pinMode(GUNNER_TOGGLE_PIN, INPUT_PULLUP);

  //gunner trigger
  //pinMode(GUNNER_TRIGGER_PIN, INPUT_PULLUP);

  delay(3000); // power-up safety delay
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);

  pinMode(DATA, OUTPUT);
  pinMode(CLOCK, OUTPUT);
  pinMode(LATCH, OUTPUT);
  pinMode(RED, OUTPUT);
  pinMode(YELLOW, OUTPUT);
}

void loop() {
  //get coms from GM
  /*while (Serial.available() > 0) {
    int inChar = Serial.read();
    
    fromCont += (char)inChar;

    if (inChar == '\n') {
      fromCont = fromCont.substring(0, fromCont.length()-2);
            
      int CommaIndex1 = fromCont.indexOf(',');
      int CommaIndex2 = fromCont.indexOf(',', CommaIndex1+1);
      int CommaIndex3 = fromCont.indexOf(',', CommaIndex2+1);
      int CommaIndex4 = fromCont.indexOf(',', CommaIndex3+1);
  
      left = fromCont.substring(0, CommaIndex1).toInt();
      right = fromCont.substring(CommaIndex1+1, CommaIndex2).toInt();
      turret = fromCont.substring(CommaIndex2+1, CommaIndex3).toInt();
      fire = fromCont.substring(CommaIndex3+1, CommaIndex4).toInt();
      sound = fromCont.substring(CommaIndex4+1).toInt();
      
      fromCont = "";
      break;
    }
  }*/

  //get all inputs
  triggerB = !digitalRead(TRIGGER_PIN);
  
  leftX = analogRead(LEFT_X_PIN);
  leftY = analogRead(LEFT_Y_PIN);

  rightX = analogRead(LEFT_X_PIN);
  rightY = analogRead(LEFT_Y_PIN);

  //Serial.println(rightX);
  
  /*gunner = !digitalRead(GUNNER_TOGGLE_PIN);
  
  if (gunner) {
    trigger = !digitalRead(GUNNER_TRIGGER_PIN);
    gunX = analogRead(GUN_X_PIN);
    gunY = analogRead(GUN_Y_PIN);
  }*/
    
  //take single joystick and map to tank controlls
  leftX = map(leftX,0,1023,-1023,1023);
  leftY = map(leftY,0,1023,1023,-1023);

  if (leftX<=X_DEAD_ZONE && leftX>=-X_DEAD_ZONE) {
    leftX = 0;
  }

  if (leftY<=Y_DEAD_ZONE && leftY>=-Y_DEAD_ZONE) {
    leftY = 0;
  }

  leftX = constrain(leftX, -JOY_MAX, JOY_MAX);
  leftY = constrain(leftY, -JOY_MAX, JOY_MAX);
  
  left = constrain(leftX - leftY,-1023,1023);
  right = constrain(leftX + leftY,-1023,1023);
  
  right *= -1; //to ajust the oreantation of the joystick


  //prepair tanks 
  if (left<0) {
    leftD = 1;
    left = left*-1;
  } else {
    leftD = 0;
  }
  left = map(left, 0, 1023, 0, 7);
  leftS = (byte)left;

  if (right<0) {
    rightD = 1;
    right = right*-1;
  } else {
    rightD = 0;
  }
  right = map(right, 0, 1023, 0, 7);
  rightS = (byte)right;
  
  //map right x axis to turret 
  //Serial.print(rightX);
  //Serial.write("\t");
  turret = map(rightX, 0, 1023, -1000, 1000)/2;
  if (turret<=TURRET_DEAD_ZONE && turret>=-TURRET_DEAD_ZONE) {
    turret = 0;
  }
  if (turret<0) {
    turretD = 1;
    turret = turret*-1;
  } else {
    turretD = 0;
  }
  //turret = map(turret, 0, 500, 0, 1000);
  //Serial.print(turret);
  //Serial.write("\t");
  turret = map(turret, 0, 500, 0, 7);
  //Serial.println(turret);
  turretS = (byte)turret;
  
  toTank[0] = B10000000|leftS<<4|leftD<<3|rightS;
  toTank[1] = B00000000|rightD<<6|turretS<<3|turretD<<2|triggerB<<1;

  //barrel
  //barrel = rightY;

  //send coms to tank
  if (millis()-milli>20)
  {
    milli = millis();
    for (int ii=0; ii<sizeof(toTank); ii++) {
      Serial.write(toTank[ii]);
    }
  }

  HealthBar(5);

  ledColour(100,0,0);
}

// 0-255
void ledColour(int redLVL, int greenLVL, int blueLVL)
{
  leds[0].setRGB(redLVL,greenLVL,blueLVL);
  FastLED.show();
}

void HealthBar(int health)
{
  //Serial.print(health);
  //Serial.write("\t");
  //Serial.println(rFlash);
  
  DisplayBar(health-2);

  if(health>=2) {
    digitalWrite(YELLOW,HIGH);
  } else {
    digitalWrite(YELLOW,LOW);
  }
  
  if(health>=1) {
    digitalWrite(RED,HIGH);
  } else if(health==0 && millis()-rFlash>100 && !rOn) {
    digitalWrite(RED,HIGH);
    rFlash = millis();
    rOn = 1;
  } else if(health==0 && millis()-rFlash>100 && rOn) {
    digitalWrite(RED,LOW);
    rFlash = millis();
    rOn = 0;
  }
}

void DisplayBar(char d)
{
  int i;

  digitalWrite(DATA,HIGH); //Put Number of LED's to turn on
  for(i=0;i<d;i++)      
  {
    digitalWrite(CLOCK,LOW);
    delay(1);
    digitalWrite(CLOCK,HIGH);
    delay(1);
  }
  
  digitalWrite(DATA,LOW); //Put remaining Zeros to keep LED's off
  for(i=0;i<(8-d);i++)       
  {
    digitalWrite(CLOCK,LOW);
    delay(1);
    digitalWrite(CLOCK,HIGH);
    delay(1);
  }  
  //Latch the data
  digitalWrite(LATCH,LOW);
  delay(1);
  digitalWrite(LATCH,HIGH);  
    
}
