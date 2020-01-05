
/****************
 * Dana Simmons 
 * 2019
 */

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>

/***********************************/
/* Configuration section           */

byte server_address[] = { 10, 1, 1, 10 };
const int port = 3000;

const char *wifi_ssid = "NachoWiFi"; //TO DO: move this to EEPROM
const char *wifi_pass = "TuxedoDrive1040";

#define SEALEVELPRESSURE_HPA (1013.25)
const int LCD_ADDRESS = 0x27;
const int BME_ADDRESS = 0x76;

const int LCD_WIDTH = 16;
const int LCD_ROWS = 2;

//const int ERROR_PIN = 2;

byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};

/**********************************/

Adafruit_BME280 bme; 
LiquidCrystal_I2C lcd(LCD_ADDRESS,LCD_WIDTH,LCD_ROWS);

/* not needed after moving to ESP8266WIFI
EthernetClient client; 
*/
WiFiClient client;

unsigned long delayTime;

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
    //pinMode(ERROR_PIN,OUTPUT);
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
    // start the Ethernet connection:
    //Ethernet.init(10);
    // Serial.println("Initialize Ethernet with DHCP:");
    // if (Ethernet.begin(mac) == 0) {
    //   Serial.println("Failed to configure Ethernet using DHCP");
    //   if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    //     Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    //   } else if (Ethernet.linkStatus() == LinkOFF) {
    //   Serial.println("Ethernet cable is not connected.");
    //   }
    //   // no point in carrying on, so do nothing forevermore:
    //   while (true) {
    //     delay(1);
    //   }
    // }
    // // print your local IP address:
    // Serial.print("My IP address: ");
    // Serial.println(Ethernet.localIP());
    
    // Serial.println("-- Default Test --");
    
    Serial.printf("Connecting to %s", wifi_ssid);
    WiFi.begin(wifi_ssid, wifi_pass);
    while(WiFi.status() != WL_CONNECTED){
      delay(500);
      Serial.print(".");
    }

    // When doen with that loop, we are connected
    Serial.println(" CONNECTED");

    delayTime = 1000;

    Serial.println();
}


void loop() { 
    printValues();
    uploadData();
    delay(delayTime);
}
void test_http(){
  char data[] = "{\"data\":\"This is a test\"}";
  // String data = "{\"data\":[";
  //   data += dataObject("temperature",bme.readTemperature());
  //   data += ",";
  //   data += dataObject("humidity",bme.readHumidity());
  //   data += "]}";
  //   client.println(data);
  HttpPostRequest req = HttpPostRequest(&client,data);
  Serial.print("Initialized request of lenghth: ");
  Serial.println(req.content_length);
  Serial.println(req.data);

  req.send(server_address, port);
}
void uploadData() {
  test_http();
  // if( client.connect( server_address , port )) {
  //   digitalWrite(ERROR_PIN,LOW);
  //   String data = "{\"data\":[";
  //   data += dataObject("temperature",bme.readTemperature());
  //   data += ",";
  //   data += dataObject("humidity",bme.readHumidity());
  //   data += "]}";
  //   client.println(data);
    
  // }else{
  //   digitalWrite(ERROR_PIN,HIGH);
  // }
}

void printValues() {
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
    Serial.println(" hPa");

    Serial.print("Approx. Altitude = ");
    Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
    Serial.println(" m");

    Serial.print("Humidity = ");
    Serial.print(bme.readHumidity());
    Serial.println(" %");

    Serial.println();
}
