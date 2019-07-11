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
#include "NarfduinoBridge.h"

// Constructors - simple stuff
NarfduinoBridge::NarfduinoBridge( byte RunPin, byte StopPin )
{
  BridgeRunPin = RunPin;
  BridgeStopPin = StopPin;
}

NarfduinoBridge::NarfduinoBridge()
{
  NarfduinoBridge( _NARFDUINOPIN_BRIDGE_RUN, _NARFDUINOPIN_BRIDGE_STOP );
}

// Init code. Set the pin modes and take them low.
bool NarfduinoBridge::Init()
{
  if( BridgeRunPin == 255 )
    return false;
  if( BridgeStopPin == 255 )
    return false;
      
  pinMode( BridgeRunPin, OUTPUT );
  digitalWrite( BridgeRunPin, LOW );  
  pinMode( BridgeStopPin, OUTPUT );
  digitalWrite( BridgeStopPin, LOW );    
  
  return true;      
}


// Start the bridge. Change the status variables - the main processor will pick up the change and run it
void NarfduinoBridge::StartBridge()
{  
  BridgeStopping = false;
  BridgeRequest = true;
  TimeLastPusherResetOrActivated = millis();
}

// Stop  the bridge. Change the status variables - the main processor will pick up the change and stop it
void NarfduinoBridge::StopBridge()
{
  BridgeRequest = false;
}

// This resets the Jam state. Bridge will not run in this state.
void NarfduinoBridge::ResetJam()
{
  JamDetected = false;
}

// Check to see if a Jam state has occurred.
bool NarfduinoBridge::HasJammed()
{
  return JamDetected;
}

// Call every time a pusher resets home. This resets the Jam timer
void NarfduinoBridge::PusherHeartbeat()
{
  TimeLastPusherResetOrActivated = millis();
}

// Set the bridge speed. 1 - 100%
void NarfduinoBridge::SetBridgeSpeed( byte NewBridgeSpeed )
{
  BridgeSpeed = NewBridgeSpeed;
}

byte NarfduinoBridge::GetBridgeSpeed()
{
  return BridgeSpeed;
}


// Turns off anti-jam detection.. For flywheels or something.
void NarfduinoBridge::DisableAntiJam()
{
  AntiJamEnabled = false;
  ResetJam();
}

// Turns on anti-jam detection. Default state
void NarfduinoBridge::EnableAntiJam()
{
  AntiJamEnabled = true;
}

// Processes the bridge and handle dead-time generation.
void NarfduinoBridge::ProcessBridge()
{
  // Logic:
  // 1 - Do we need to change? If so, initiate transition
  // 2 - Are we in transition? If so, write digital 0 to both fets. Keep doing that
  // 3 - Are we out of transition? If so, 
  // 4.a  - Are we wanting to run? Are we already running or just out of transition?
  //        If so, turn on Bridge fet, ensure brake fet is off
  //        Else turn fets off, and start transition timer
  // 4.b  - Are we wanting to stop? Are we already stoppedor just out of transition?
  //        If so, turn off Bridge fet, and turn on brake fet
  //        Else turn fets off and start transition timer


  // Step 1 - initiate transition
  if( LastBridgeRequest != BridgeRequest )
  {
    CurrentBridgeStatus = _NARFDUINO_BRIDGE_TRANSITION;
    if( BridgeRequest )
    {
      SelectedTransitionTime = _NARFDUINO_BRIDGE_ON_TRANSITION_TIME;
    }
    else
    {
      SelectedTransitionTime = _NARFDUINO_BRIDGE_OFF_TRANSITION_TIME;
    }
    BridgeTransitionStart = millis();
    LastBridgeRequest = BridgeRequest;
    return;
  }

  // Step 2 - wait for transition to complete. This is the dead-time.
  if( CurrentBridgeStatus == _NARFDUINO_BRIDGE_TRANSITION )
  {
    digitalWrite( BridgeRunPin, LOW );
    BridgePWMFETOn = false;
    digitalWrite( BridgeStopPin, LOW );
    BridgeBrakeFETOn = false;
    if( millis() - BridgeTransitionStart <= SelectedTransitionTime )
      return;

    // We are now out of transition
    if( BridgeRequest )
    {
      CurrentBridgeStatus = _NARFDUINO_BRIDGE_RUN;
    }
    else
    {
      CurrentBridgeStatus = _NARFDUINO_BRIDGE_STOP;
    }
    
    return;
  }

  // Bridge should now turn on.
  if( CurrentBridgeStatus == _NARFDUINO_BRIDGE_RUN )
  {
    // Check to make sure high fet is not on
    if( BridgeBrakeFETOn )
    {
      return;
    }
    // Check to make sure that the Jam state isn't set
    if( JamDetected )
    {
      digitalWrite( BridgeRunPin, LOW );
      digitalWrite( BridgeStopPin, LOW );
      return;
    }
    // Check to see if we have Jammed, if anti-jam is turned on. If so, halt the bridge and set the Jam state.
    if( AntiJamEnabled )
    {
      if( (millis() - TimeLastPusherResetOrActivated) > _NARFDUINO_PUSHER_MAX_CYCLE_TIME )
      {
        // Jam detected, shut down fets.
        digitalWrite( BridgeRunPin, LOW );
        digitalWrite( BridgeStopPin, LOW );
        JamDetected = true;
        return;
      }
      else
      {
        JamDetected = false;
      }
    }
    else
    {
      JamDetected = false;
    }

    // Ensure that the high side stays off.
    digitalWrite( BridgeStopPin, LOW );
    BridgeBrakeFETOn = false;

    // Handle a few different speed conditions.
    if( BridgeSpeed == 100 )
    {
      // Digital write HIGH for 100%
      digitalWrite( BridgeRunPin, HIGH );
    }
    else if( BridgeSpeed == 0 )
    {
      // Just in case we want 0%
      digitalWrite( BridgeRunPin, LOW );
    }
    else
    {
      // PWM Write
      int PWM = map( BridgeSpeed, 0, 100, 0, 255 );
      analogWrite( BridgeRunPin, PWM );
    }
    BridgePWMFETOn = true;
    
    return;
  }

  // Bridge should now turn off
  if( CurrentBridgeStatus == _NARFDUINO_BRIDGE_STOP )
  {
    // Check to make sure low fet is not on
    if( BridgePWMFETOn )
    {
      return;
    }

    // Ensure the low fet stays off
    digitalWrite( BridgeRunPin, LOW );
    BridgePWMFETOn = false;

    // Activate the brake
    digitalWrite( BridgeStopPin, HIGH );
    BridgeBrakeFETOn = true;

    return;
  }
}
