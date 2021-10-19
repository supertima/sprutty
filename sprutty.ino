#include <Wire.h>                     //  Connect lib to use I2C bus (for display)
#include <LiquidCrystal_I2C.h>        //  Connect lib to use Display on I2C bus

#include <OneWire.h>                  //  Connect lib to use OneWire protocol, needed for Dallas DS18B20 sensors
#include <DallasTemperature.h>        //  Connect lib to use Dallas DS18B20 sensors

#define ONE_WIRE_BUS 10               //  Here we define which digital pin DS18B20 sensors array will use

OneWire oneWire(ONE_WIRE_BUS);        //  Setup a oneWire instance to communicate with any OneWire devices
DallasTemperature sensors(&oneWire);  //  Pass our oneWire reference to Dallas Temperature.

// Here we define known addresses of 3 DS18B20s. If you don't know them, there are plenty of arduino sketches, which scan OneWire bus and output results to the Serial port
uint8_t sensor1[8] = { 0x28, 0xFF, 0x64, 0x1E, 0x01, 0xA6, 0xAF, 0x6A };
uint8_t sensor2[8] = { 0x28, 0xFF, 0x64, 0x1E, 0x01, 0xAF, 0xDD, 0x9C };
uint8_t sensor3[8] = { 0x28, 0xFF, 0x64, 0x1E, 0x01, 0xA0, 0xE8, 0x05 };

LiquidCrystal_I2C lcd(0x27,20,4);     //  Here we define symbol display parameters (address of I2C = 0x27, columns = 20, rows = 4)

unsigned int rpm;
double timefunc;
                              
void setup()
{                      
    sensors.begin();                  //  Initialize sensors
     
    lcd.init();                       //  Initialize LCD display
    lcd.backlight();                  //  Power on LCD backlight 
    lcd.clear();                      //  Clear LCD of everything
    lcd.setCursor( 0, 0);             //  Set cursor on first symbol of first row. (Numeration begin from "0").
    lcd.print("Oil temp:");           //  Print description on LCD. It is printed in the beginning of the first row.
    lcd.setCursor( 0, 1);             //  Set cursor on first symbol of second row. (Numeration begin from "0").
    lcd.print("Engine temp:");        //  Print description on LCD. It is printed in the beginning of the second row.
    lcd.setCursor( 0, 2);             //  Set cursor on first symbol of third row. (Numeration begin from "0").
    lcd.print("Radiator temp:");      //  Print description on LCD. It is printed in the beginning of the third row.
    lcd.setCursor( 0, 3);             //  Set cursor on first symbol of fourth row. (Numeration begin from "0").
    lcd.print("Engine RPM:");         //  Print description on LCD. It is printed in the beginning of the fourth row.
      
    attachInterrupt(0, rpm_count, RISING);  //  Setup an interrupt which activates "rpm" function each time value on first interrupt (D2 pin) rises.
      
    revolutions = 0;                  //  Stores number of revolutions between calculation cycles
    rpm = 0;                          //  Needed for calculations
    timeold = 0;                      //  Stores timestamp for the end of previous calculation cycle
    timefunc = 0;                     //  Stores timestamp for the beginning of current calculation cycle
}                                    

void loop()
{
  sensors.requestTemperatures();      // call "sensors.requestTemperatures()" to issue a global temperature request to all devices on the bus  

  lcd.setCursor( 15, 0);              // Set cursor after the previously printed "Oil temp:" on LCD.
  printTemperature(sensor1);          // Print sensor's 1 temperature on LCD.
  lcd.setCursor( 15, 1);              // Set cursor after the previously printed "Engine temp:" on LCD.
  printTemperature(sensor2);          // Print sensor's 2 temperature on LCD.
  lcd.setCursor( 15, 2);              // Set cursor after the previously printed "Radiator temp:" on LCD.
  printTemperature(sensor3);          // Print sensor's 3 temperature on LCD.

  if ((micros()-timeprev)>2000000)    // If there is more than 2 seconds since last rev, we consider the engine is stopped
  {
     lcd.setCursor( 15, 3);
     lcd.print("STOP");      
   }
  
  else                                // else we just print RPM's on the display
  {
     lcd.setCursor( 16, 3);
     lcd.print("   ");
     lcd.setCursor( 15, 3);
     lcd.print(rpm); 
  }  
}


// Function to be activated during interrupt to count rpm's. We count RPM's inside interrupt as is it is much faster and more precise.
void rpm_count()                            
{
  rpm = 60/((micros()-timeprev)/1000000);   // Math is simple: we calculate how much time passed since previous rev. Knowing that, we can easily get how many RPM's will engine make in 60 seconds.
  timeprev = micros();
}

// Function to be activated during interrupt to count rpm's
void rpm_count()                            
{
  revolutions++;
}

// Here we define a universal function, which receives sensor's address as a parameter, checks it's temperature and print's it on LCD. 
void printTemperature(DeviceAddress deviceAddress)    
{
  float tempC = sensors.getTempC(deviceAddress);      // Check what is the temperature on the sensor with the address "deviceAddress" and sets this value to the variable "tempC".
  lcd.print(tempC);                                   // Print tempC's value on the LCD. Note: this function prints text wherever cursors is. So you have to set cursor's location beforehand. 
}
