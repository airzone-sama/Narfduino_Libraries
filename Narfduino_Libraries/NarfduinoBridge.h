/*
 *  Narfduino Libraries - NarfduinoBridge
 *  
 *  Use this to operate the Half-H-Bridge on the Narfduino
 *  Ideally used for a pusher mechanism, but could be used for anything else
 *  Can be used on other boards or with multiple half-h-bridges on a single board.
 *  
 *  !!!!! WARNING !!!!!
 *  This library handles deadtime generation for the bridge. Modify at your own risk!
 *  Without appropriate deadtime, explosions will happen. Possibly fire. 
 *  
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
 *    - Especially if you change it and blow up your board.
 *  
 */

#include "Arduino.h"

// Default Definitions

// This is the time it takes for the Stop FETs to discharge in ms
#ifndef _NARFDUINO_BRIDGE_ON_TRANSITION_TIME
  #define _NARFDUINO_BRIDGE_ON_TRANSITION_TIME 10 
#endif

// This is the time it takes for the Run FET to discharge in ms
#ifndef _NARFDUINO_BRIDGE_OFF_TRANSITION_TIME
  #define _NARFDUINO_BRIDGE_OFF_TRANSITION_TIME 2 
#endif

// This is the Pusher Reset Switch heartbeat interval time in ms. 
// If we don't hear it, we have probably stalled the pusher. Stop the bridge before something burns out.
#ifndef _NARFDUINO_PUSHER_MAX_CYCLE_TIME
  #define _NARFDUINO_PUSHER_MAX_CYCLE_TIME 500   
#endif


// Pin Definitions
// Gate for the N Fet
#ifndef _NARFDUINOPIN_BRIDGE_RUN
  #define _NARFDUINOPIN_BRIDGE_RUN 5
#endif
// Gate for the P Det Driver
#ifndef _NARFDUINOPIN_BRIDGE_STOP
  #define _NARFDUINOPIN_BRIDGE_STOP 15
#endif


// Internal flags
#define _NARFDUINO_BRIDGE_STOP 0     // Bridge brake is on, PWM is off
#define _NARFDUINO_BRIDGE_TRANSITION 1  // Bridge brake is off, PWM is off, waiting for fet caps to discharge
#define _NARFDUINO_BRIDGE_RUN 2         // Bridge brake is off, PWM is on

class NarfduinoBridge
{
    public:

      // Constructors
      NarfduinoBridge();
      NarfduinoBridge( byte RunPin, byte StopPin ); // Use if you want to override the default pins - such as adding an additional bridge
      

      // Initialisation function - Call only once in your startup code
      bool Init();


      // **********************************
      // Status and configuration functions
      // **********************************
      
      // Determines if the pusher has jammed.
      bool HasJammed();
      
      // Find out what the current bridge speed is - from 1 to 100 
      byte GetBridgeSpeed();
      
      // Set the bridge speed - from 1 to 100.
      void SetBridgeSpeed( byte NewBridgeSpeed );

      // Turns off anti-jam detection.. For flywheels or something.
      void DisableAntiJam();

      // Turns on anti-jam detection. Default state
      void EnableAntiJam();

      
      // ********************
      // Processing functions
      // ********************
      
      // Starts the bridge - only call this when you want to start the bridge when it's stopped.
      void StartBridge();
      
      // Stops the bridge - call this any time.
      void StopBridge();
      
      // Resets the Jam state.. The bridge will stop if a jam is detected
      void ResetJam();
      
      // Advise the pusher heartbeat - this is your pusher reset switch. 
      // This needs to be called every time you have a pusher reset otherwise the anti-jam will trigger (if enabled)
      void PusherHeartbeat();

      // Handles the bridge processing. 
      // This needs to be called on regular intervals - such as every time through your main loop
      // Handles dead-time generation and state changes. Bridge won't do anything without this running.
      void ProcessBridge();


      // Private stuff
    private:
      bool LastBridgeRequest = true;
      int SelectedTransitionTime = 100;
      bool BridgePWMFETOn = false; // Keep track of the PWM FET
      bool BridgeBrakeFETOn = false; // Keep track of the brake fet
      bool BridgeRequest = false; // False = stop Bridge, true = run Bridge
      byte CurrentBridgeStatus = _NARFDUINO_BRIDGE_TRANSITION; // Start in transition mode for boot.
      unsigned long BridgeTransitionStart = 0; // Time transition started
      unsigned long TimeLastPusherResetOrActivated = 0; // We are keeping track when the pusher was last reset for anti-jam purposes.
      bool JamDetected = false;
      bool BridgeStopping = false;
      byte BridgeSpeed = 0; // ROF Percentage
      byte BridgeRunPin = 255;
      byte BridgeStopPin = 255;
      bool AntiJamEnabled = true; 
};
