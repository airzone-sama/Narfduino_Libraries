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

#include "NarfduinoBrushless.h"

// Call once in setup to initialise the pins and OC1 timer.
void NarfduinoBrushless::Init()
{
  // Set the pins accordingly.
  #ifdef _NARFDUINO_ENABLE_BRUSHLESS_9
    pinMode( _NARFDUINO_PIN_MOTOR_9, OUTPUT );
    digitalWrite( _NARFDUINO_PIN_MOTOR_9, LOW );
  #endif

  #ifdef _NARFDUINO_ENABLE_BRUSHLESS_10
    pinMode( _NARFDUINO_PIN_MOTOR_10, OUTPUT );
    digitalWrite( _NARFDUINO_PIN_MOTOR_10, LOW );
  #endif

  // Initialise the PWM registers for OC1 
  TCCR1A = 0;
  TCCR1A = (1 << WGM11);
  #ifdef _NARFDUINO_ENABLE_BRUSHLESS_9
    TCCR1A |= (1 << COM1A1);
  #endif
  #ifdef _NARFDUINO_ENABLE_BRUSHLESS_10
    TCCR1A |= (1 << COM1B1);
  #endif
  TCCR1B = 0;
  TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11);
  ICR1 = 40000;

  // Pulse 1000us
  UpdateSpeed( 1000 );
}

// Updates the PWM Timers. Take a value from 1000 - 2000us, and adjust it to a value that the timer can use.
void NarfduinoBrushless::UpdateSpeed( int NewSpeed )
{
  NewSpeed = (NewSpeed * 2) + 2; // Adjust for the prescalar
  
  #ifdef _NARFDUINO_ENABLE_BRUSHLESS_9
    OCR1A = NewSpeed;
  #endif
  #ifdef _NARFDUINO_ENABLE_BRUSHLESS_10
    OCR1B = NewSpeed;
  #endif
}
