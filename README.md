# sprutty
A simple arduino project to check engine temperature and rev's (tachometer). It's based on Arduino UNO, three DS18B20 sensors, one analog TCRT5000 sensor and 20x4 I2C symbol display.

## Temperature
This part is pretty easy: sprutty constantly asks sensors for temperature value over OneWire bus. Sensors are predefined by address, so sprutty does not scan the bus, but requests predefined sensors.

##Tachometer magic
I chose "interrupts" technique: each time white mark passes in front of TCRT5000 sensor, it raises the value on the signal pin. This raise is detected by interrupt, which pauses the whole program, adds "1" to the revolutions counter and falls back to normal.
