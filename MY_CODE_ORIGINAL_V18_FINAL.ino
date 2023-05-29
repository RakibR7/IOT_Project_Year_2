//Rakib Rana G00408644

//Thingspeak baseline - Temp and humidity sensor copied
//working with thingspeak
//temperature humidity sensor/motor/leds
//<iframe src="https://giphy.com/embed/TfFnane40TevvZS9iT" width="480" height="480" frameBorder="0" class="giphy-embed" allowFullScreen></iframe><p><a href="https://giphy.com/gifs/animation-after-effects-studioaka-TfFnane40TevvZS9iT">via GIPHY</a></p>

/**********
  Rui Santos this guys work is used mostly
  Complete project details at https://randomnerdtutorials.com  
*********/

/*
 * This ESP32 code is created by esp32io.com
 *
 * This ESP32 code is released in the public domain
 *
 * For more detail (instruction and wiring diagram), visit https://esp32io.com/tutorials/esp32-light-sensor
 
// only use 33,32,34,35 adc1 pins or else it wont work with wifi
//3.3V
 */

// To create literal strings stored in flash memory enclose your HTML code between 
// F(R"=====( HTML code here )=====");

//Primary Hex colour #44C1A3
//background-color: #DFDBE5;


//Theres use of mdn for javascript funcitons
#include <WiFi.h>
#include <WebServer.h>
#include <Servo.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include "homepage.h"

WebServer server(80);

const char* ssid = "VODAFONE-F473";
const char* password = "1234567890";

//Temp and Humidity Sensor
#define DHTPIN            14
#define DHTTYPE DHT11

//LDR (light-dependent-diode)
#define LDR_PIN           34
#define LED_PIN           22
#define ANALOG_THRESHOLD  1000
int lightState = 0;
//0 for auto
int controlMode = 0;    
int currentPosition = 0;
int servoPosition = 0;  // 0 for closed, 1 for open
int servoControlMode = 0; // 0 for auto, 1 for manual


//DHT sensor creating a object
DHT dht(DHTPIN, DHTTYPE);
WiFiClient client;

//Servo pin
const int servoPin = 13;
//from servo library, to write an angle
Servo myservo;
 
void handleDHTFetch();
void handleLDRFetch();
void handleServoFetch();
void handleRoot();
void handleNotFound();
void handleToggleLight();
void handleAutoManualSwitch();
void automaticLightControl();
void handleServoControl(); 
void controlServoWithTemperature();
void handleServoModeSwitch();


void setup() {
  Serial.begin(115200);

  dht.begin();
  myservo.attach(servoPin);
  //myservo2.attach(servoPin2); 
  //myservo3.attach(servoPin3);
  WiFi.mode(WIFI_STA);
  //set up route to server, get is waiting for incoming request, then next handleDHTFetch will be executed
  pinMode(LED_PIN, OUTPUT);
  server.on("/", handleRoot);
  server.on("/getDHT", handleDHTFetch);
  server.on("/getLDR", handleLDRFetch);
  server.on("/getServo", handleServoFetch);
  server.on("/servo", handleServoControl);
  //handlers: switches between auto and manual control modes.
  server.on("/LightMode", handleToggleLight);
  //visuall
  server.on("/switchMode", handleAutoManualSwitch);
  server.on("/switchServoMode", handleServoModeSwitch);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("Server Started");

  WiFi.begin(ssid, password);
  Serial.println("");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  //manages the req, needed t maintain server, 
  server.handleClient();
  delay(2);

  if (controlMode == 0) {
    automaticLightControl();
  }

  controlServoWithTemperature();
}

void handleRoot() {
  String webPage = homePagePart1;
  webPage.replace("Temp_PlaceHolder", String(readDHTTemperature())); 
  webPage.replace("Humidity_PlaceHolder", String(readDHTHumidity()));
  webPage.replace("Light_PlacHolder", String(analogRead(LDR_PIN)));
  webPage.replace("Servo_PlaceHolder", String(myservo.read()));
  server.send(200, "text/html", webPage);
}


void handleDHTFetch() {
  String dhtStr = String(readDHTTemperature()) + " " + String(readDHTHumidity());
  server.send(200, "text/plain", dhtStr);
}

void handleLDRFetch() {
  String ldrStr = String(analogRead(LDR_PIN));
  server.send(200, "text/plain", ldrStr);
}

void handleServoFetch() {
  String servoStr = String(myservo.read());
  server.send(200, "text/plain", servoStr);
}


void handleNotFound() {
  server.send(404, "text/plain", "Not found");
}

float readDHTTemperature() {
  float temperatureC = dht.readTemperature();
  Serial.print("Temperature (ºC): ");
  Serial.println(temperatureC);
  return temperatureC;
}

float readDHTHumidity() {
  float humidityC = dht.readHumidity();
  Serial.print("Humidity (%): ");
  Serial.println(humidityC);
  return humidityC;
}

int readLDR()
{
  int analogValue = analogRead(LDR_PIN);
  Serial.print("LDR (%): ");
  Serial.println(analogValue);
  return analogValue;
}

void handleToggleLight() {
  if (lightState) {
    digitalWrite(LED_PIN, LOW);
    lightState = 0;
    server.send(200, "text/plain", "off");
  } else {
    digitalWrite(LED_PIN, HIGH);
    lightState = 1;
    server.send(200, "text/plain", "on");
  }
}


void handleAutoManualSwitch() {
  if (controlMode == 1) {
    controlMode = 0;
    server.send(200, "text/plain", "auto");
  } else {
    controlMode = 1;
    server.send(200, "text/plain", "manual");
  }
}


void automaticLightControl() {
  int ldrValue = readLDR();
  if (ldrValue < ANALOG_THRESHOLD) {
    digitalWrite(LED_PIN, HIGH);
  } else {
    digitalWrite(LED_PIN, LOW);
  }
}

//controls Serv0 m0t0r
void handleServoControl() {
  if(servoControlMode == 1) { // Manual mode
    if(currentPosition == 0) {
      currentPosition = 45; // move to position 45,90
    } 
    else 
    {
      currentPosition = 0; 
    }
    myservo.write(currentPosition);
    server.send(200, "text/plain", "Servo moved");
  }
}


void controlServoWithTemperature() {
  if(servoControlMode == 0) { // Auto mode
    float currentTemperature = readDHTTemperature();
    if(currentTemperature <= 20){
      myservo.write(180);
      Serial.print("\nDoor open ");
    }      
    else
    {
      myservo.write(0);   
      Serial.print("\nDoor closed ");
    }
  }
}

//handles auto and manual
void handleServoModeSwitch() {
  if (servoControlMode == 1) {
    servoControlMode = 0;
    server.send(200, "text/plain", "auto");
  } else {
    servoControlMode = 1;
    //rsepnse back t0 the client
    server.send(200, "text/plain", "manual");
  }
}

