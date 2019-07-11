// NarfduinoBrushless Example
// Connect ESC to pin 9 or 10. No other pins will work.

// These need to be defined above the #include. This will enable brushless control on the respective pin
#define _NARFDUINO_ENABLE_BRUSHLESS_9
#define _NARFDUINO_ENABLE_BRUSHLESS_10

#include "NarfduinoBrushless.h"

// Create instance of library
NarfduinoBrushless Brushless = NarfduinoBrushless();


void setup() {
  // put your setup code here, to run once:

  // Initialise the PWM Timers
  Brushless.Init();

  // To arm the ESC, pull speed to 1000 and wait a few seconds
  Brushless.UpdateSpeed( 1000 );
  delay(3000);
}

void loop() {
  // put your main code here, to run repeatedly:

  // Spin up to 20% for 2 seconds
  Brushless.UpdateSpeed( 1200 );
  delay( 2000 );

  // Brake hard for 2 seconds. SimonK will coast to a stop, BLHeli_x will come to a screetching halt. This causes heat buildup in the motor
  Brushless.UpdateSpeed( 1000 );
  delay( 2000 );
}
