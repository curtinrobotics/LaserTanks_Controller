int left;
int leftD;
byte leftB;

int right;
int rightD;
byte rightB;

byte drive;

int turret;
int turretD;
byte turretB;

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
int trigger = 1;
int controllSwitch;

int gunner;
int gunRight;
int gunLeft;

long int milli = 0;

//analog pins
#define LEFT_X_PIN 4
#define LEFT_Y_PIN 5
#define RIGHT_X_PIN 2
#define RIGHT_Y_PIN 3
#define GUN_X_PIN 4
#define GUN_Y_PIN 5

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

void setup() {
  //usb serial
  Serial.begin(9600);

  //wifi serial
  Serial1.begin(9600);
  
  //trigger
  pinMode(TRIGGER_PIN, INPUT_PULLUP);

  pinMode(ENABLE, OUTPUT);
  digitalWrite(ENABLE, HIGH);
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
  trigger = !digitalRead(TRIGGER_PIN);
  
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
  
  //right *= -1; //to ajust the oreantation of the joystick

  if (left<0) {
    leftD = 1;
    left = left*-1;
  } else {
    leftD = 0;
  }
  left = map(left, 0, 1023, 0, 7);

  leftB = ((byte)left)<<5;
  leftB = leftB|(((byte)leftD)<<4);

  if (right<0) {
    rightD = 1;
    right = right*-1;
  } else {
    rightD = 0;
  }
  right = map(right, 0, 1023, 0, 7);

  rightB = ((byte)right)<<1;
  rightB = rightB|((byte)rightD);

  toTank[0] = leftB|rightB;
  
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

  turretB = ((byte)turret)<<5;
  turretB = turretB|(((byte)turretD)<<4);
  
  turretB = turretB|(((byte)trigger)<<3);
  toTank[1] = turretB;//|(((byte)1)<<7);

   

  //barrel
  //barrel = rightY;

  //send coms to tank
  /*Serial.print(left);
  Serial.write(",");
  Serial.print(right);
  Serial.write(",");
  Serial.print(turret);
  Serial.write(",");
  Serial.print(barrel);
  Serial.write(",");
  Serial.print(red);
  Serial.write(",");
  Serial.print(green);
  Serial.write(",");
  Serial.print(blue);
  Serial.write(",");
  Serial.print(trigger);*/
  //Serial.write(",");
  //Serial.print(sound);
  //Serial.write(",");
  //Serial.print(powerUpUse);

  //Serial.write("\t");
  Serial.print(toTank[0],BIN);
  Serial.write("\t");
  Serial.println(toTank[1],BIN);
  //Serial.write("\n ");
  //Serial.write("\t");
  //Serial.write(toTank[1]);
  //Serial.write("\n");

  //-1023-1023-1000102315
  
  /*sprintf(toTank, "%05d%05d%05d%04d%d%d", left, right, turret, barrel, trigger, sound);
  Serial.println(toTank);
  Serial1.println(toTank);*/

  
  /*Serial1.print(left); //-1023 to 1023 0 to 2046
  Serial1.write(",");
  Serial1.print(right); //-1023 to 1023
  Serial1.write(",");
  Serial1.print(turret); //0 to 1023
  Serial1.write(",");
  Serial1.print(barrel); //0 to 1023
  Serial1.write(",");*/
  /*Serial1.print(red);
  Serial1.write(",");
  Serial1.print(green);
  Serial1.write(",");
  Serial1.print(blue);
  Serial1.write(",");*/
  /*Serial1.print(trigger); //1
  Serial1.write(",");
  Serial1.println(sound); //5*/
  //Serial1.write(",");
  //Serial1.print(powerUpUse);
  //Serial.write(toTank,2);
  //Serial1.write(B01110100);//toTank[0]);
  //Serial1.write("k");//toTank[1]);
  if (millis()-milli>50)
  {
    milli = millis();
    Serial1.write(toTank[0]);
    Serial1.write(toTank[1]);
  }
  
  if (Serial1.available()) {     // If anything comes in Serial1 (pins 0 & 1)
    Serial.write(Serial1.read());   // read it and send it out Serial (USB)
  }

  /*if (Serial.available()) {     // If anything comes in Serial1 (pins 0 & 1)
    Serial1.write(Serial.read());   // read it and send it out Serial (USB)
  }*/
  //delay(45);
}


// 010 1 101 1
