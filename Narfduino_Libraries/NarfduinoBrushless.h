/*
 *  Narfduino Libraries - NarfduinoBrushless
 *  
 *  Use this to easily control brushless motors using embedded hardware. Servo sucks, kids.
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

#include "Arduino.h"

// Uncomment these (or add to your header) to enable brushless / brushed ESC's on the respective pins.
// #define _NARFDUINO_ENABLE_BRUSHLESS_9
// #define _NARFDUINO_ENABLE_BRUSHLESS_10

// The pins are fixed.
#define _NARFDUINO_PIN_MOTOR_9 9
#define _NARFDUINO_PIN_MOTOR_10 10


class NarfduinoBrushless
{
  public:

    // Call once in setup to initialise the pins and OC1 timer.
    void Init();
    
    // Updates the PWM Timers. Take a value from 1000 - 2000us, and adjust it to a value that the timer can use.
    void UpdateSpeed( int NewSpeed );

};
