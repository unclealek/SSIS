#define BLYNK_TEMPLATE_ID ""
#define BLYNK_TEMPLATE_NAME ""
#define BLYNK_AUTH_TOKEN ""
#define BLYNK_PRINT Serial
#include "DHTesp.h"
#include <I2CSoilMoistureSensor.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char ssid[] = "";  //Enter your WIFI SSID
char pass[] = "";  //Enter your WIFI Password

DHTesp dht;
#define tankLED 13//Tank Light (WHEN TANK IS EMPTY)
#define DC_Pump 12 // relay pin for pump
#define tank 14 

#define Moisture A0 //ADC moisture pin
int SOIL_SENSOR_1;  // initialize soil sensor to 0
bool pumpStatus = false; // keep pump off at start of machine
int pump_trigger = 400;  //pump threshold, ie if soil moisture = pump or greter than turn the pump onand stay on until below pump threshold
bool tank_led = false; 
void setup()
{
  Serial.begin(115200);
  pinMode(tank, INPUT);
  pinMode(tankLED,OUTPUT);
  pinMode(DC_Pump, OUTPUT);
  digitalWrite(DC_Pump , pumpStatus);
  dht.setup(5, DHTesp::DHT22);;

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, "blynk.cloud", 80);
}

//Get the DHT22 sensor values
void dht22se() {
  TempAndHumidity measurement = dht.getTempAndHumidity();
  float t = measurement.temperature;
  float h = measurement.humidity;
  Blynk.virtualWrite(V1, t);
  Blynk.virtualWrite(V2, h);
  Serial.print("Temperature: ");
  Serial.println(t);
  Serial.print("Humidity: ");
  Serial.println(h);
}

void TANK(){
  bool value = digitalRead(tank);
  if (value==1) {
        tankLed_On();
        Serial.println("NO WATER DETECTED !!!!");    
        
      } else {
        tankLed_Off();
        Serial.println("wATER dETECTED");
        
      }
      Blynk.virtualWrite(V5, tank);
  }
void tankLed_Off() {
    digitalWrite(tankLED, LOW);
    tank_led = false;
    delay(2000);
    if (tank_led) {
      Blynk.logEvent("EmptyTank","wATER Detected!!!"); //Enter your Event Name
      WidgetLED LED(V5);
      LED.off();
      Serial.println("Tank Full");
    }
  }

  void tankLed_On() {
    digitalWrite(tankLED, HIGH);
    tank_led = true;
    delay(2000);
    if (!tank_led) {
      Blynk.logEvent("EmptyTank","WARNNG! NO WATER DETECTED !!!!"); //Enter your Event Name
      WidgetLED LED(V5);
      LED.on();
      Serial.println("Tank Empty");
    }
}

void SOIL_per(){
  SOIL_SENSOR_1 = analogRead(Moisture);
  /*SOIL_SENSOR_1 = map(value, 0, 600, 0, 100);
  SOIL_SENSOR_1 = (SOIL_SENSOR_1 - 100) * -1;*/
  Blynk.virtualWrite(V0, SOIL_SENSOR_1);
  Serial.println(SOIL_SENSOR_1);
  delay(1000);
  if (SOIL_SENSOR_1 >= pump_trigger) {
      pumpOn();
    } else {
      pumpOff();
    }
    Blynk.virtualWrite(V4, pumpStatus);
  }
  void pumpOn() {
    digitalWrite(DC_Pump, HIGH);
    pumpStatus = true;
    delay(2000);
    if (pumpStatus) {
      Serial.println("-Pump On,");
    }
  }

  void pumpOff() {
    digitalWrite(DC_Pump, LOW);
    pumpStatus = false;
    delay(2000);
    if (!pumpStatus) {
      Serial.println("-Pump Off,");
    }
}

void loop()
{
  TANK();
  dht22se();
  SOIL_per(); 
  Blynk.run();//Run the Blynk library
  
}