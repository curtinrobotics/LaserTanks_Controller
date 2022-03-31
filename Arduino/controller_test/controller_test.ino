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

int leftX;
int leftY;
int rightX;
int rightY;

long int milli = 0;

//analog pins
//for the old controller mockup
#define LEFT_X_PIN 4
#define LEFT_Y_PIN 5
#define RIGHT_X_PIN 2
#define RIGHT_Y_PIN 3
#define GUN_X_PIN 4
#define GUN_Y_PIN 5

#define X_DEAD_ZONE 40
#define Y_DEAD_ZONE 40
#define TURRET_DEAD_ZONE 100
#define BARREL_DEAD_ZONE 40
#define JOY_MAX 1023

void setup() {
  //usb serial
  Serial.begin(9600);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
}

void loop() {
  //get all inputs  
  leftX = analogRead(LEFT_Y_PIN);
  leftY = analogRead(LEFT_X_PIN);
  
  //Serial.print(leftY);
  //Serial.write("\t");

  rightX = analogRead(LEFT_X_PIN);
  rightY = analogRead(LEFT_Y_PIN);
    
  //take single joystick and map to tank controlls
  analogWrite(10, map(leftX,0,1023,0,255));
  analogWrite(11, map(leftY,0,1023,0,255));

  Serial.print(map(leftX,0,1023,-100,100));
  Serial.write("\t");
  Serial.print(map(leftY,0,1023,-100,100));
  Serial.write("\t");
  Serial.print(100);
  Serial.write("\t");
  Serial.print(-100);
  Serial.write("\n");
  
  leftX = map(leftX,0,1023,-1023,1023);
  leftY = map(leftY,0,1023,-1023,1023);
  //leftY = map(leftY,0,1023,1023,-1023);
  
  //Serial.print(leftY);
  //Serial.write("\t");

  if (leftX<=X_DEAD_ZONE && leftX>=-X_DEAD_ZONE) {
    leftX = 0;
  }

  if (leftY<=Y_DEAD_ZONE && leftY>=-Y_DEAD_ZONE) {
    leftY = 0;
  }
  
  //Serial.print(leftY);
  //Serial.write("\t");

  leftX = constrain(leftX, -JOY_MAX, JOY_MAX);
  leftY = constrain(leftY, -JOY_MAX, JOY_MAX);
  
  //Serial.print(leftY);
  //Serial.write("\t");
  
  left = constrain(leftX - leftY,-1023,1023);
  right = constrain(leftX + leftY,-1023,1023);
  
  right *= -1; //to ajust the oreantation of the joystick

  //map right x axis to turret 
  turret = map(rightX, 0, 1023, -1000, 1000);
  if (turret<=TURRET_DEAD_ZONE && turret>=-TURRET_DEAD_ZONE) {
    turret = 0;
  }
}

