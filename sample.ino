/***********************
 * Sample class implementations
 * Dana Simmons 2019
 */



Sample::Sample(Adafruit_BME280 *bme, bool METRIC){

  if(METRIC){
    temperature = bme->readTemperature();
    pressure = bme->readPressure() / 100.0F;
    altitude = bme->readAltitude(SEALEVELPRESSURE_HPA);
    humidity = bme->readHumidity();
  }else{
    temperature = (bme->readTemperature() * 1.8) + 32;
    pressure = (bme->readPressure() / 100.0F) * 1.333;
    altitude = bme->readAltitude(SEALEVELPRESSURE_HPA) * 3.281 ;
    humidity = bme->readHumidity();
  }
}

Sample::Sample(DHT *dht, bool METRIC){
  temperature = dht->readTemperature(!METRIC);
  pressure = 0;
  altitude = 0;
  humidity = dht->readHumidity();
}
