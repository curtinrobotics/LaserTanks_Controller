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

//analog pins
//for the old controller mockup
#define LEFT_X_PIN 4
#define LEFT_Y_PIN 5
#define RIGHT_X_PIN 2
#define RIGHT_Y_PIN 3
#define GUN_X_PIN 4
#define GUN_Y_PIN 5

/* For the new pcb
#define RIGHT_X_PIN 0
#define RIGHT_Y_PIN 1
#define LEFT_X_PIN 2
#define LEFT_Y_PIN 3
#define GUN_X_PIN 4
#define GUN_Y_PIN 5*/

//digital pins
#define TRIGGER_PIN 2
#define GUNNER_TRIGGER_PIN 3
#define GUNNER_TOGGLE_PIN 4
#define POWEUP_PIN 5

//digital out pins
#define RED_PIN 4
#define GREEN_PIN 5
#define BLUE_PIN 6

#define X_DEAD_ZONE 40
#define Y_DEAD_ZONE 40
#define TURRET_DEAD_ZONE 100
#define BARREL_DEAD_ZONE 40
#define JOY_MAX 1023

//wifi
#define RESET 10
#define ENABLE 11

void prntBits(byte b, char lnEnd='\0');

void setup() {
  //usb serial
  Serial.begin(9600);

  //wifi serial
  Serial1.begin(9600);
  
  //trigger
  pinMode(TRIGGER_PIN, INPUT_PULLUP);

  pinMode(ENABLE, OUTPUT);
  digitalWrite(ENABLE, HIGH); //not sure y this is not just shorted to 5V
  pinMode(RESET, OUTPUT);
  digitalWrite(RESET, LOW);
  delay(200);
  digitalWrite(RESET, HIGH);
  
  //gunner toggle
  //pinMode(GUNNER_TOGGLE_PIN, INPUT_PULLUP);

  //gunner trigger
  //pinMode(GUNNER_TRIGGER_PIN, INPUT_PULLUP);
}

void loop() {
  //get coms from GM
  /*while (Serial1.available() > 0) {
    int inChar = Serial1.read();
    
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
  turret = map(rightX, 0, 1023, -1000, 1000);
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
  turret = map(turret, 0, 1000, 0, 7);
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
      Serial1.write(toTank[ii]);
    }

    for (int ii=0; ii<sizeof(toTank)-1; ii++) {
      prntBits(toTank[ii], '\t');
    }
    prntBits(toTank[sizeof(toTank)-1], '\n');
  }
  
  if (Serial1.available()) {     // If anything comes in Serial1 (pins 0 & 1)
    //Serial.write(Serial1.read());   // read it and send it out Serial (USB)
  }

  /*if (Serial.available()) {     // If anything comes in Serial1 (pins 0 & 1)
    Serial1.write(Serial.read());   // read it and send it out Serial (USB)
  }*/
  //delay(45);
}

void prntBits(byte b, char lnEnd='\0')
{
  for(int i = 7; i >= 0; i--)
    Serial.print(bitRead(b,i));
  Serial.write(lnEnd);
}
