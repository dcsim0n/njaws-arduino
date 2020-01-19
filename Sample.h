/**********************
 * Sample class definitions
 * Dana Simmons 2020
 */



class Sample {
  public:
  Sample(Adafruit_BME280 *bme, bool METRIC);
  bool METRIC;
  float temperature;
  float pressure;
  float humidity;
  float altitude;
};
