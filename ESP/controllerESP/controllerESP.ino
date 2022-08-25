//libraries
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#include <PubSubClient.h>

//constants
#define DEBUG 1
#define DELAY 500
#define SERIAL_SPEED 9600
#define AP_NAME "lasertanks"
#define AP_PASS "lasertanks"
#define UDP_PORT 100

const IPAddress GATEWAY(192,168,8,1);
const IPAddress SUBNET(255,255,0,0);

const IPAddress GM_IP(192,168,8,2);

#define CONTROLLER_NUM 3

const IPAddress CONTROLLER_IP(192,168,8,(CONTROLLER_NUM*10)+1);
const IPAddress TANK_IP(192,168,8,(CONTROLLER_NUM*10)+2);
#define HOSTNAME "Controller " + CONTROLLER_NUM

const char* mqtt_server = "192.168.8.2";
const String MQTT_PARENT =      "tank/" + String(CONTROLLER_NUM);

const char* MQTT_HITBY =        (MQTT_PARENT + "/hitBy").c_str();
const char* MQTT_POWERUPUSED =  (MQTT_PARENT + "/powerupUsed").c_str();

const char* MQTT_STATUS =       (MQTT_PARENT + "/status").c_str();
const char* MQTT_ALIVE =        (MQTT_PARENT + "/alive").c_str();
const char* MQTT_HEALTH =       (MQTT_PARENT + "/health").c_str();
const char* MQTT_POWERUP =      (MQTT_PARENT + "/powerup").c_str();

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

//variables
WiFiUDP udp;

//functions
void setup(){
  delay(DELAY);
  startSerial();
  connectToAP();
  startUDP();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  delay(DELAY);

  Serial.println(MQTT_PARENT);
  Serial.println(MQTT_HITBY);
  Serial.println(MQTT_POWERUPUSED);
}

void loop(){
  sendControlData();
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}

//functions
void startSerial(void){
  Serial.begin(SERIAL_SPEED);
  #ifdef DEBUG
    Serial.println("Serial ready");
  #endif
}

void startUDP(void){
  boolean result = (boolean)udp.begin(UDP_PORT);
  #ifdef DEBUG
    if(result == true)
    {
      Serial.println("UDP ready");
    }
    else
    {
      Serial.println("UDP failed");
    }
  #endif
}

void connectToAP(void){
  boolean result = false;

  WiFi.hostname(HOSTNAME);
  WiFi.begin(AP_NAME, AP_PASS);
  WiFi.setAutoReconnect(true);
  
  result = WiFi.config(CONTROLLER_IP, GATEWAY, SUBNET);
  #ifdef DEBUG
    if(result == true)
    {
      Serial.println("Configuration ready");
    }
    else
    {
      Serial.println("Configuration failed");
    }

    Serial.println("Connecting");
  #endif
  while(WiFi.status() != WL_CONNECTED)
  {
    delay(DELAY);
  }
  #ifdef DEBUG
    Serial.println("Connected");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    Serial.println(WiFi.macAddress());
  #endif
}

void sendControlData(void){
  if(Serial.available() > 0)
  {    
    byte readByte = Serial.read();
    
    udp.beginPacket(TANK_IP, UDP_PORT);
    udp.write(readByte);
    udp.endPacket();
    #ifdef DEBUG 
      Serial.print("Controller->Tank - ");
      Serial.write(readByte);
      Serial.println();
    #endif
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  String topicStr = (String)topic;
  //String payloadStr = (String)(char*)payload;

  if (topicStr.endsWith("alive")) {
    Serial.println((char*)payload);
  }

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {//, MQTT_STATUS, 1, 0, (char*)"disconnected")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(MQTT_STATUS, "connected");
      // ... and resubscribe
      client.subscribe(MQTT_ALIVE);
      client.subscribe(MQTT_HEALTH);
      client.subscribe(MQTT_POWERUP);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
