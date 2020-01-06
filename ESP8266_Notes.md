# Pin connections setup

```
VCC----> 3.3v
RST----> NC
CH_PD--> 3.3v
DTXU---> TX (Pin 1)
URXD---> RX (Pin 0)
GPIO_0-> GND
GPIO_2-> NC
GND----> GND
```

# Flashing steps
1. On arduino, ground reset pin to disable atmega chip
2. connect ESP8266 pins as in section 1
3. Chose COM port, generic ESP
4. Upload
5. when finished disconnect GPIO_0
6. Leave CH_PD connected to boot firmware, aka make it work
7. Collect spoils



# I2C docs
+ [Arduino Wire Library](https://www.arduino.cc/en/reference/wire)
  
  Doesn't show optional pin reference, maybe Wire has a separate implementation for ESP8266?
+ [Adafruit I2C pin selection](https://learn.sparkfun.com/tutorials/esp8266-thing-hookup-guide/using-the-arduino-addon)
  
  This shows calling `Wire.being([pin_sda],[pin_scl]);` will over ride defaults
+ Pins should be the GPIO pin numbers not physical pins. IE `Wire.begin(0,2)` to use `GPIO 0` and `GPIO 2` for I2C

+ [Arduino JSON Library docs](https://arduinojson.org/)

  Wonderfull library for managing JSON de/serialization
