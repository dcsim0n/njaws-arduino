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
