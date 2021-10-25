# Sprutty
A simple arduino project to check engine temperature and rev's (tachometer). It's based on Arduino UNO, three DS18B20 sensors, one analog TCRT5000 sensor and 20x4 I2C symbol display.

## Temperature
This part is pretty easy: sprutty constantly asks sensors for temperature value over OneWire bus. Sensors are predefined by address, so sprutty does not scan the bus, but requests predefined sensors.

## Tachometer magic
I chose "interrupts" technique: each time white mark passes in front of TCRT5000 sensor, it raises the value on the signal pin. This raise is detected by interrupt, which pauses the whole program, recalculates RPM and falls back to normal. As far as i discovered this is the most accurate way to count rpm's.

## Wiring
I used a plastic box with a 3D-printed base to host DC-DC 12>7 volts power adapter and Arduino Nano (Iskra Nano Pro, actually). On the bottom side I have 6 sockets: power, display, tcrt5000 sensor and 3 sockets for DS18B20 sensors. I should've used 4.7 kOhm resistor instead of 2.2, but this still works.

All externals drain +5v from arduino, so I soldered them at one bundle, throwing a single wire to arduino's +5v pin. Same is done for ground wires and DS18B20 data wires.

![Internals of the box](IMG_7442.jpg?raw=true "Internals of the box")
