/****************
 * Dana Simmons 
 * 2019
 */

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>

#include <ESP8266WebServer.h>


#include <ArduinoJson.h>

/***********************************/
/* Configuration section           */

byte server_address[] = { 10, 1, 1, 10 };
const int port = 3000;

const char *wifi_ssid = "NachoWiFi"; //TO DO: move this to EEPROM
const char *wifi_pass = "TuxedoDrive1040";


const int LCD_ADDRESS = 0x27;
const int BME_ADDRESS = 0x76;

const int LCD_WIDTH = 16;
const int LCD_ROWS = 2;

const float SEALEVELPRESSURE_HPA = 1013.25;
/**********************************/

Adafruit_BME280 bme; 
LiquidCrystal_I2C lcd(LCD_ADDRESS,LCD_WIDTH,LCD_ROWS);

/* not needed after moving to ESP8266WIFI
EthernetClient client; 
*/
WiFiClient client;

unsigned long LAST_LOG = millis();
int LOG_INTERVAL = 2000; //ms 
ESP8266WebServer server(80);


class HttpPostRequest {
  public:
  
  WiFiClient *client;
  char *data;
  int content_length;
  int send(byte *server_address, int port);
  HttpPostRequest(WiFiClient *client, char *data);
  String dataObject(String key_name,float value);
};

void setup() {
    Serial.begin(9600);
    while(!Serial);    // time to get serial running
    Serial.println(F("BME280 test"));

    unsigned status;
    
    Wire.begin(0,2); // Set I2C sda & sck pins
    status = bme.begin(BME_ADDRESS,&Wire);
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0,0);
    lcd.print("Init...");
    // You can also pass in a Wire library object like &Wire2
    // status = bme.begin(0x76, &Wire2)
    if (!status) {
        Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
        Serial.print("SensorID was: 0x"); Serial.println(bme.sensorID(),16);
        Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
        Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
        Serial.print("        ID of 0x60 represents a BME 280.\n");
        Serial.print("        ID of 0x61 represents a BME 680.\n");
        lcd.print("Err");
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

    server.on("/",handleRoot);
    server.begin();
    
    Serial.println("HTTP Started!");
}


void loop() { 
    printValues();
    server.handleClient();
}

void handleNotFound() {
  //digitalWrite(led, 1);
  String message = "{\"error\":\"Not Found\"}";
  server.send(404, "application/json", message);
  //digitalWrite(led, 0);
}
void handleRoot(){
  Serial.println("Handling request: /");
  float temp = bme.readTemperature();
  float pres = bme.readPressure() / 100.0F;
  float alti = bme.readAltitude(SEALEVELPRESSURE_HPA);
  float humi = bme.readHumidity();

  // Build Json Document 
  const int capacity = JSON_OBJECT_SIZE(5);
  
  StaticJsonDocument<capacity> json_response;
  json_response["temperature"] = temp;
  json_response["pressure"] = pres;
  json_response["altitude"] = alti;
  json_response["humidity"] = humi;
  json_response["time"] = millis();

  // Prepare string buffer for response
  int response_length = 1 + measureJson(json_response);

  char response_str[response_length];
  
  serializeJson(json_response,response_str,response_length);

  // Send response with status code
  server.send(200,"application/json",response_str);

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
