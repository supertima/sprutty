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

// Tachometer BLOCK

volatile unsigned long pulseCount = 0;       // Counter for pulses detected
unsigned long lastTime = 0;                  // Last time RPM was calculated
const int sensorPin = 2;                     // IR sensor connected to digital pin 2 (interrupt pin)
const int sampleTime = 1000;                 // Sample time in milliseconds (1 second)
const int pulsesPerRevolution = 1;           // Adjust based on your setup
float rpm = 0;                               // Calculated RPM
const int debounceDelay = 5;                 // Minimum time between pulses (ms) to filter noise
volatile unsigned long lastPulseTime = 0;    // Time of the last valid pulse

// Moving average filter variables
const int numSamples = 5;                    // Number of RPM samples to average
float rpmSamples[numSamples];                // Array to store RPM samples
int sampleIndex = 0;                         // Index for the sample array



void setup()
{                      
    sensors.begin();                  //  Initialize sensors

    pinMode(sensorPin, INPUT);                 // Set sensor pin as input
    attachInterrupt(digitalPinToInterrupt(sensorPin), countPulse, FALLING); // Interrupt on falling edge
    Serial.begin(9600);                        // Start serial communication
    Serial.println("Tachometer Started");

    // Initialize RPM samples array
    for (int i = 0; i < numSamples; i++)
      {
        rpmSamples[i] = 0;
      }
  
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
}                                    

void printTemperature(DeviceAddress deviceAddress)    
{
  float tempC = sensors.getTempC(deviceAddress);      // Check what is the temperature on the sensor with the address "deviceAddress" and sets this value to the variable "tempC".
  lcd.print(tempC);                                   // Print tempC's value on the LCD. Note: this function prints text wherever cursors is. So you have to set cursor's location beforehand. 
}

void loop()
{
  // TEMP DISPLAY BLOCK
  sensors.requestTemperatures();      // call "sensors.requestTemperatures()" to issue a global temperature request to all devices on the bus  

  lcd.setCursor( 15, 0);              // Set cursor after the previously printed "Oil temp:" on LCD.
  printTemperature(sensor1);          // Print sensor's 1 temperature on LCD.
  lcd.setCursor( 15, 1);              // Set cursor after the previously printed "Engine temp:" on LCD.
  printTemperature(sensor2);          // Print sensor's 2 temperature on LCD.
  lcd.setCursor( 15, 2);              // Set cursor after the previously printed "Radiator temp:" on LCD.
  printTemperature(sensor3);          // Print sensor's 3 temperature on LCD.


  // REV DISPLAY BLOCK
  unsigned long currentTime = millis();      // Get current time

  // Calculate RPM every 'sampleTime' milliseconds
  if (currentTime - lastTime >= sampleTime) {
    noInterrupts();                          // Disable interrupts to safely read pulseCount
    unsigned long pulses = pulseCount;       // Store pulse count
    pulseCount = 0;                          // Reset pulse count
    interrupts();                            // Re-enable interrupts

    // Calculate instantaneous RPM
    float instantRPM = (float)pulses / pulsesPerRevolution * (60000.0 / sampleTime);

    // Update the moving average
    rpmSamples[sampleIndex] = instantRPM;
    sampleIndex = (sampleIndex + 1) % numSamples; // Circular buffer

    // Calculate average RPM
    float sum = 0;
    for (int i = 0; i < numSamples; i++) {
      sum += rpmSamples[i];
    }
    rpm = sum / numSamples;

    // Print RPM to Serial Monitor
    Serial.print("RPM: ");
    Serial.println(rpm);

    lastTime = currentTime;                  // Update lastTime
  }
  
  if (sampleTime>2000)    // If there is more than 2 seconds since last rev, we consider the engine is stopped
  {
     lcd.setCursor( 15, 3);
     lcd.print("STOP");      
   }
  
  else                                // else we just print RPM's on the display
  {
     lcd.setCursor( 15, 3);
     lcd.print("     ");
     lcd.setCursor( 15, 3);
     lcd.print(rpm); 
  }  
}
