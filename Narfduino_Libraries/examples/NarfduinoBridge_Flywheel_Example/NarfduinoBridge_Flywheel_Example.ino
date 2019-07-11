// Example of NarfduinoBridge for flywheel / non-return pusher use
// The demo will spin the flywheels at 33% for 3 seconds, then stop for 3 second, then 66% for 3 seconds, then stop for 3 seconds, then 100% for 3 seconds, then stop for 3 seconds, and repeat.

// Include the library
#include "NarfduinoBridge.h"

// Create our bridge object
NarfduinoBridge Flywheels = NarfduinoBridge();
// n.b. You can also use NarfduinoBriodge( x, y ); where x is the PWM enabled pin for "Go" (low side fet) and y is the digital pin for "Stop" (high side fet)


void setup() {

  // Initialise the library
  Flywheels.Init();

  // Since we are running in flywheel mode, you need to disable the jam detection.
  Flywheels.DisableAntiJam();
}

void loop() {
  // Load in some variables here.
  static byte DesiredSpeed = 100;
  static bool IsRunning = false;
  static unsigned long LastChange = 0;

  // Change state every 3 seconds
  if( millis() - LastChange > 3000 )
  {
    LastChange = millis();
    
    
    // Every 3 seconds, change the state of the bridge. 
    // Every time it runs, run it 33% faster than before, and start again when you cap 100%
    if( IsRunning )
    {
      // Signal the bridge to stop.
      Flywheels.StopBridge();

      // Update our tracking flags
      IsRunning = false;
    }
    else // Flywheels are stopped, start them now.
    {
      // Update our tracking flags
      IsRunning = true;

      // Prepare a new speed for the next time the 
      if( DesiredSpeed == 33 )
        DesiredSpeed = 66;
      else if( DesiredSpeed == 66 )
        DesiredSpeed = 100;
      else
        DesiredSpeed = 33;

      // Configure a new speed
      Flywheels.SetBridgeSpeed( DesiredSpeed );

      // Signal the bridge to run
      Flywheels.StartBridge();
    }
  }

  // Run this basically every loop. It will control the dead time
  Flywheels.ProcessBridge();
  
}
