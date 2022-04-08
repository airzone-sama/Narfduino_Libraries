/*
 *  Narfduino Libraries - NarfduinoBattery
 *  
 *  Use this to manage battery monitoring on the Narfduino
 *  Can be used on other boards with the battery connected through a 10k & 47k voltage divider on pin A7
 *  
 *  (c) 2019 - Ireland Software
 *  License: 
 *    - You are free to use this software for non-commercial use.
 *    - If you purchase a Narfduino board from Airzone's Blasters (or other authorised supplier), 
 *      then you can use this software commercially for software loaded on that board.
 *    - Commercial use on other boards requires a paid license
 *    - Modifications to the software follow this license
 *    
 *  Warranty:
 *    - No warranty, expressed or implied. Software is provided as-is. Use at own risk.
 *  
 */

#ifndef _NARFDUINO_BATTERY_LIB
#define  _NARFDUINO_BATTERY_LIB
 
#include "Arduino.h"

// Default Definitions

// The Arduino pin to use
#ifndef _NARFDUINO_PIN_BATTERY
  #define _NARFDUINO_PIN_BATTERY A7
#endif

// This is to rate-limit the battery check. Default is 500ms - the voltage will be read with a min of 500ms before the next read
#ifndef _NARFDUINO_BATTERY_CHECK_INTERVAL
  #define _NARFDUINO_BATTERY_CHECK_INTERVAL 500
#endif

// This is to check the number of samples before the voltage is calculated. Increase to improve accuracy and ride out sags. Decrease to get a snappier battery read
#ifndef _NARFDUINO_BATTERY_NUM_SAMPLES
  #define _NARFDUINO_BATTERY_NUM_SAMPLES 6
#endif

// This can be used to provide a constant calibration adjustment... To take into account deviation with the resistors, trace resistance, etc
#ifndef _NARFDUINO_BATTERY_CALFACTOR
  #define _NARFDUINO_BATTERY_CALFACTOR 0.0
#endif


// This defines the Min and Max voltage thresholds for 2s, 3s, and 4s batteries
#ifndef _NARFDUINO_BATTERY_2S_MIN
  #define _NARFDUINO_BATTERY_2S_MIN 6.5
#endif
#ifndef _NARFDUINO_BATTERY_2S_MAX
  #define _NARFDUINO_BATTERY_2S_MAX 8.4
#endif
#ifndef _NARFDUINO_BATTERY_3S_MIN
  #define _NARFDUINO_BATTERY_3S_MIN 9.5
#endif
#ifndef _NARFDUINO_BATTERY_3S_MAX
  #define _NARFDUINO_BATTERY_3S_MAX 13.0
#endif
#ifndef _NARFDUINO_BATTERY_4S_MIN
  #define _NARFDUINO_BATTERY_4S_MIN 13.1
#endif
#ifndef _NARFDUINO_BATTERY_4S_MAX
  #define _NARFDUINO_BATTERY_4S_MAX 16.8
#endif


// Header definitions

class NarfduinoBattery
{
  public:
    // Constructors   
    NarfduinoBattery( byte _BatteryPin ); // Use if you want to override the default pin.
    NarfduinoBattery();


    // ***************************************
    // Initialisation Functions - Use in Setup
    // ***************************************
    
    // Call this once, to initialise the library.
    bool Init(); 
    
    // Auto-detect the BatteryS based on a series of reads.
    // Warning: This is blocking. Only use it in initialisation code.
    void SetupSelectBattery();
    
    // Sets the Battery S. Use this if you want to manually define the BatteryS instead of auto detecting it.
    void SetBatteryS( byte NewBatteryS );


    // ************************************
    // Runtime Functions - Call as required
    // ************************************
    
    // Returns the current battery voltage. Will return 99.0 if the voltage hasn't been calculated yet.
    float GetCurrentVoltage();
    
    // Gets the current BatteryS 
    byte GetBatteryS();
    
    // Gets the current percentage of battery remaining
    byte GetBatteryPercent();
    
    // Gets an indication if the battery is flat
    bool IsBatteryFlat();
    
    // Run the battery monitor. This needs to be run at regular intervals.
    void ProcessBatteryMonitor();

  private:
    byte BatteryPin = 255;
    unsigned long LastCheck = 0;
    float BatteryCurrentVoltage = 99.0;
    bool BatteryFlat = false;
    byte BatteryPercent = 100;
    float BatteryMaxVoltage;
    float BatteryMinVoltage;
    byte BatteryS = 3;    
};

#endif 