// Demo of NarfduinoBattery

// Include the library
#include "NarfduinoBattery.h"


// You can optionally change the calibration by adding / subtracting voltage. 
// The monitor may not be 100% accurate.
#define _NARFDUINO_BATTERY_CALFACTOR 0.0

// Initialise the library
NarfduinoBattery Battery = NarfduinoBattery();

void setup() {
  // Serial debugging output
  Serial.begin( 57600 );

  // Initialise the library
  Battery.Init();
  // Detect the battery size here. 
  Battery.SetupSelectBattery();
  // Alternatively use .SetBatteryS( x ); where x is the number of S in the battery

  // Display the result of the detection
  Serial.print( "Battery Connected S = " );
  Serial.println( Battery.GetBatteryS() );
  
}

void loop() {
  // We need to keep constant track of the battery. The library will rate limit itself.  
  // However we should rate limit output to the serial port
  static unsigned long LastTimeDisplayed = 0;

  // Run this frequently
  Battery.ProcessBatteryMonitor();

  if( millis() - LastTimeDisplayed > 1000 )
  {
    LastTimeDisplayed = millis();

    //Show the Voltage
    Serial.print( "Battery Voltage = " );
    Serial.println( Battery.GetCurrentVoltage() );

    // Show the % left
    Serial.print( "Battery % = " );
    Serial.println( Battery.GetBatteryPercent() );

    // Is the battery flat yet?
    Serial.print( "Battery Flat = " );
    Serial.println( Battery.IsBatteryFlat() );            
  }

}
