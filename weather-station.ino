/*************************
  Dana Simmons 
  2019
  Version 1.3
 ************************/

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServerSecure.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include "Sample.h"
/***********************************/
/* Configuration section           */

// Create wifi_secret.h with wifi_ssid and wifi_pass char arrays
#include "wifi_secret.h"

const bool LCD_ENABLE = true;
const int LCD_ADDRESS = 0x27;
const int BME_ADDRESS = 0x76;

const int LCD_WIDTH = 16;
const int LCD_ROWS = 2;
const int LOG_INTERVAL = 4000; //ms

const float SEALEVELPRESSURE_HPA = 1013.25;

/* Initialize Global State */

Adafruit_BME280 bme; 

LiquidCrystal_I2C lcd(LCD_ADDRESS,LCD_WIDTH,LCD_ROWS);

WiFiClient client;

unsigned long LAST_LOG = millis(); 
ESP8266WebServerSecure secure_server(443);
ESP8266WebServer server(80);
void setup() {
    Serial.begin(9600);
    while(!Serial);    // time to get serial running
    Serial.println(F("BME280 test"));

    unsigned status;
    
    Wire.begin(0,2); // Set I2C sda & sck pins
    status = bme.begin(BME_ADDRESS,&Wire);
    
    if(LCD_ENABLE){
      lcd.init();
      lcd.backlight();
      lcd.setCursor(0,0);
      lcd.print("Init...");
    }
    // You can also pass in a Wire library object like &Wire2
    // status = bme.begin(0x76, &Wire2)
    if (!status) {
        Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
        Serial.print("SensorID was: 0x"); Serial.println(bme.sensorID(),16);
        Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
        Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
        Serial.print("        ID of 0x60 represents a BME 280.\n");
        Serial.print("        ID of 0x61 represents a BME 680.\n");
        if(LCD_ENABLE){
          lcd.print("Err");  
        }
        while (1) delay(10);
    }

    // Setup WiFi connection next
    //
    Serial.printf("Connecting to %s", wifi_ssid);
    WiFi.begin(wifi_ssid, wifi_pass);
    while(WiFi.status() != WL_CONNECTED){
      delay(500);
      Serial.print(".");
    }

    // When doen with that loop, we are connected
    
    Serial.print(" CONNECTED:");
    Serial.println(WiFi.localIP());

    Serial.println();
    Serial.println("Starting web server...");

//    secure_server.getServer().setServerKeyAndCert_P(rsakey, sizeof(rsakey), x509, sizeof(x509));
//    secure_server.on("/",handleRoot);
//    secure_server.on("/i",handleImperial);
//    secure_server.begin();
    
    server.on("/",handleRoot);
    server.on("/i",handleImperial);
    server.begin();
    
    Serial.println("HTTP Started!");
}


void loop() { 
    printValues();
    server.handleClient();
    secure_server.handleClient();
}
  
int buildJsonString(char *response_str, Sample *sample){
  // Build Json Document 
  const int capacity = JSON_OBJECT_SIZE(5);
  
  StaticJsonDocument<capacity> json_response;
  json_response["temperature"] = sample->temperature;
  json_response["pressure"] = sample->pressure;
  json_response["altitude"] = sample->altitude;
  json_response["humidity"] = sample->humidity;
  json_response["time"] = millis();

  // Prepare string buffer for response
  int response_length = 1 + measureJson(json_response);
  // char response_str[response_length];
  serializeJson(json_response,response_str,response_length);
  
  return 0;
}

void handleNotFound() {
  //digitalWrite(led, 1);
  String message = "{\"error\":\"Not Found\"}";
  server.send(404, "application/json", message);
  //digitalWrite(led, 0);
}

void handleRoot(){
  Serial.println("Handling request: /");

  Sample sample = Sample(&bme,true);
  char response[240] = "";
  buildJsonString(response, &sample);
  // String response = buildJsonString(&sample);
  server.send(200,"application/json",response);

}

void handleImperial(){
  Serial.println("Handling request: /i");

  Sample sample = Sample(&bme,false);
  char response[240] = "";
  buildJsonString(response,&sample);
  // String response = buildJsonString(&sample);
  Serial.println(response);
  server.send(200,"application/json",response);

}

void printValues() {
    if(sinceLastLog() < LOG_INTERVAL){
      return;
    }
    LAST_LOG = millis();
    Serial.print("Temperature = ");
    String temp = String(bme.readTemperature(),1);
    String pres = String((bme.readPressure() / 100.0F),1);
    String alti = String(bme.readAltitude(SEALEVELPRESSURE_HPA),0);
    String humi = String(bme.readHumidity(),1);
    
    Serial.print(temp);
    Serial.println(" *C");
    
    if(LCD_ENABLE){ 
      lcd.setCursor(0,0);
      lcd.print(temp);
      lcd.print("*C |");
      lcd.print(pres);
      lcd.print("hPa ");
      lcd.setCursor(0,1);
      lcd.print(alti);
      lcd.print("m   | ");
      lcd.print(humi);
      lcd.print("%");
    }
    
    Serial.print("Pressure = ");

    Serial.print(bme.readPressure() / 100.0F);


    Serial.print("Approx. Altitude = ");
    Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
    Serial.println(" m");

    Serial.print("Humidity = ");
    Serial.print(bme.readHumidity());
    Serial.println(" %");

    Serial.println();
}

unsigned long sinceLastLog(){
  return millis() - LAST_LOG;
}
