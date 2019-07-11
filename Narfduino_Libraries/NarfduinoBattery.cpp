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


#include "NarfduinoBattery.h"


NarfduinoBattery::Narfduino( byte _BatteryPin )
{
  BatteryPin = _BatteryPin;
}

NarfduinoBattery::Narfduino()
{
  Narfduino( _NARFDUINO_PIN_BATTERY );
}

// Call this once, to initialise the library.
bool NarfduinoBattery::Init()
{
  // Somehow the battery pin hasn't been set.
  if( BatteryPin == 255 )
    return false;

  pinMode( BatteryPin, INPUT );
  return true;
}

// Returns the current battery voltage. Will return 99.0 if the voltage hasn't been calculated yet.
float NarfduinoBattery::GetCurrentVoltage()
{
  return BatteryCurrentVoltage;
}

// Gets the current BatteryS 
byte NarfduinoBattery::GetBatteryS()
{
  return BatteryS;
}

// Gets the current percentage of battery remaining
byte NarfduinoBattery::GetBatteryPercent()
{
  return BatteryPercent;
}

// Gets an indication if the battery is flat
bool NarfduinoBattery::IsBatteryFlat()
{
  return BatteryFlat;
}

// Sets the Battery S. Use this if you want to manually define the BatteryS instead of auto detecting it.
void NarfduinoBattery::SetBatteryS( byte NewBatteryS )
{
  BatteryS = NewBatteryS;
  switch( BatteryS )
  {
    case 4:
      BatteryMaxVoltage = _NARFDUINO_BATTERY_4S_MAX;
      BatteryMinVoltage = _NARFDUINO_BATTERY_4S_MIN;
      break;
    case 3:
      BatteryMaxVoltage = _NARFDUINO_BATTERY_3S_MAX;
      BatteryMinVoltage = _NARFDUINO_BATTERY_3S_MIN;
      break;
    default:
      BatteryMaxVoltage = _NARFDUINO_BATTERY_2S_MAX;
      BatteryMinVoltage = _NARFDUINO_BATTERY_2S_MIN;
  }      
}

// Run the battery monitor. This needs to be run at regular intervals.
void NarfduinoBattery::ProcessBatteryMonitor()
{
  // Check Battery every 500ms
  if( millis() - LastCheck < _NARFDUINO_BATTERY_CHECK_INTERVAL )
  {
    return;
  }
  LastCheck = millis();

  static byte CollectedSamples = 0;
  static float SampleAverage = 0;
  float SensorValue = analogRead( _NARFDUINO_PIN_BATTERY );
  if( CollectedSamples < _NARFDUINO_BATTERY_NUM_SAMPLES )
  {
    CollectedSamples ++;
    SampleAverage += SensorValue;
  }
  else
  {
    BatteryCurrentVoltage = (((float)SampleAverage / (float)CollectedSamples * 5.0)  / 1024.0 * (float)((47.0 + 10.0) / 10.0)) + (float)_NARFDUINO_BATTERY_CALFACTOR;  // Voltage dividor - 47k and 10k
    if( BatteryCurrentVoltage < BatteryMinVoltage )
    {
      if( BatteryCurrentVoltage > 1.6 ) // If the current voltage is 0, we are probably debugging
      {
        BatteryFlat = true;
      }
      else
      {
        BatteryFlat = false;
      }
    }
    else
    {
      BatteryFlat = false;
    } 
    BatteryPercent = map( (int)(BatteryCurrentVoltage * 10), (int)(BatteryMinVoltage * 10), (int)(BatteryMaxVoltage * 10), 1, 100 );
    CollectedSamples = 0;
    SampleAverage = 0;
  }
}


// Auto-detect the BatteryS based on a series of reads.
// Warning: This is blocking. Only use it in initialisation code.
void NarfduinoBattery::SetupSelectBattery()
{
  #define SAMPLE_DELAY 10

  byte CollectedSamples = 0;
  float SampleAverage = 0;

  for( byte c = 0; c < _NARFDUINO_BATTERY_NUM_SAMPLES; c++)
  {
    float SensorValue = analogRead( _NARFDUINO_PIN_BATTERY );
    CollectedSamples ++;
    SampleAverage += SensorValue;
    delay( SAMPLE_DELAY );
  }

  BatteryCurrentVoltage = (((float)SampleAverage / (float)CollectedSamples * 5.0)  / 1024.0 * (float)((47.0 + 10.0) / 10.0)) + (float)_NARFDUINO_BATTERY_CALFACTOR;  // Voltage dividor - 47k and 10k
  
  if( BatteryCurrentVoltage < _NARFDUINO_BATTERY_4S_MIN )
  {
    if( BatteryCurrentVoltage < _NARFDUINO_BATTERY_3S_MIN )
      SetBatteryS( 2 );
    else
      SetBatteryS( 3 );
  }
  else
    SetBatteryS( 4 );

}
    
