





/*************************
  Dana Simmons 
  2019
  Version 1.3
 ************************/

#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServerSecure.h>
#include <ESP8266WebServer.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_BME280.h>
#include <DHT.h>
#include <DHT_U.h>
#include <ArduinoJson.h>
#include "Sample.h"
#include <Adafruit_SSD1306.h>
#include <splash.h>

/***********************************/
/* Configuration section           */

// Create wifi_secret.h with wifi_ssid and wifi_pass char arrays
#include "wifi_secret.h"

//const int BME_ADDRESS = 0x76;  // Address of the Sensor

#define DHT_TYPE DHT22
const int DHT_PIN = 2;


const bool LCD_ENABLE = true;  // If no LCD attached set to false
const int LCD_ADDRESS = 0x3c;  // Address of the LCD controller
const int LCD_WIDTH = 128;
const int LCD_ROWS = 64;
const int LOG_INTERVAL = 4000; // miliseconds between log output

const float SEALEVELPRESSURE_HPA = 1013.25;

/*-----------------------------------------------------------------------*/
// Initialize global variables here
/*-----------------------------------------------------------------------*/

// Uncomment for bme sensors
//Adafruit_BME280 bme; 

// Uncoment for DHT22 sensors
DHT dht(DHT_PIN, DHT_TYPE);

// Uncomment for 16x2 lcd
//LiquidCrystal_I2C lcd(LCD_ADDRESS,LCD_WIDTH,LCD_ROWS);

// Configure OLED display
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(LCD_WIDTH, LCD_ROWS, &Wire, OLED_RESET);

WiFiClient client;
unsigned long LAST_LOG = millis(); 
ESP8266WebServer server(80);

// Begin initialization
void setup() {
    Serial.begin(9600);
    while(!Serial);    // time to get serial running
    
    // Serial.println(F("BME280 test"));

    unsigned status;
    
    // Configure pins to use for I2C buss
    // Comment this out to use default I2C pins
    // Only necessary for 8 pin ESP10 modules that have only 2 gpio pins
    // Wire.begin(0,2); // Set I2C sda & sck pins
    // status = bme.begin(BME_ADDRESS,&Wire);
    dht.begin();
    
    status = display.begin(SSD1306_SWITCHCAPVCC, LCD_ADDRESS);
    
    if(!status){
      Serial.println("Could not initialize display and sensors");
      while(1){
        delay(200);
      }
    }

    display.clearDisplay();
    display.setCursor(0,0);
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);        // Draw white text
    display.println("Connecting to: ");
    display.println(wifi_ssid);
    display.display();

    // Setup WiFi connection next
    Serial.printf("Connecting to %s", wifi_ssid);
    WiFi.begin(wifi_ssid, wifi_pass);
    while(WiFi.status() != WL_CONNECTED){
      delay(500);
      Serial.print(".");
    }

    // When doen with that loop, we are connected
    Serial.print(" CONNECTED:");
    display.println("Connected:");
    display.println(WiFi.localIP());
    display.display();
    delay(2000);\
    // Log the IP address so we can connect later
    Serial.println(WiFi.localIP());

    Serial.println();
    Serial.println("Starting web server...");
    
    // Setup method handlers for two different types of units
    server.on("/",handleRoot);
    server.on("/i",handleImperial);
    server.begin();
    
    Serial.println("HTTP Started!");
}


void loop() { 
    printValues();
    server.handleClient();
}
  
void buildJsonString(char *response_str, Sample *sample){
  // Build Json Document to store response in
  const int capacity = JSON_OBJECT_SIZE(5);
  StaticJsonDocument<capacity> json_response;

  // set key names and assign data
  json_response["temperature"] = sample->temperature;
  json_response["pressure"] = sample->pressure;
  json_response["altitude"] = sample->altitude;
  json_response["humidity"] = sample->humidity;
  json_response["time"] = millis();

  // Prepare string buffer for response
  int response_length = 1 + measureJson(json_response);

  // serialize json object and copy data into response_str variable
  serializeJson(json_response,response_str,response_length);
}

// Notify users that we don't know how to handle their request
void handleNotFound() {
  String message = "{\"error\":\"Not Found\"}";
  server.send(404, "application/json", message);
}

void handleRoot(){
//  Sample sample = Sample(&bme,true);
  Sample sample = Sample(&dht, true);
  char response[100] = "";
  buildJsonString(response, &sample);
  server.send(200,"application/json",response);
}

void handleImperial(){
//  Sample sample = Sample(&bme,false);
  Sample sample = Sample(&dht, false);
  char response[100] = "";
  buildJsonString(response,&sample);
  Serial.println(response);
  server.send(200,"application/json",response);
}

void printValues() {
    
    if(!is_time_to_log()){
      // exit without logging if the it's not time for it yet
      return;
    }
    
    Sample sample = Sample(&dht, false);

    Serial.print("Temperature = ");
    Serial.println(sample.temperature);
    Serial.print("Humidity = ");
    Serial.println(sample.humidity);

    display.clearDisplay();
    display.setCursor(0,0);
    display.setTextSize(2);
    display.print("T: ");
    display.println(sample.temperature);
    display.println();
    display.print("H: ");
    display.println(sample.humidity);
    display.setTextSize(1);
    display.println();
    display.println(WiFi.localIP());
    display.display();
    
}

bool is_time_to_log(){
  bool is_time_to_log = false;
  if((millis() - LAST_LOG) > LOG_INTERVAL){
      is_time_to_log = true;
      LAST_LOG = millis();
    }
  return is_time_to_log;
}
