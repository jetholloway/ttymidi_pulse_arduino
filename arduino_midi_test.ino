#include "/home/jet/tmp/arduino_midi_test/MIDI/MIDI.h"

struct MySettings : public midi::DefaultSettings
{
  static const bool UseRunningStatus = false; // Messes with my old equipment!
};

MIDI_CREATE_CUSTOM_INSTANCE(HardwareSerial, Serial, MIDI, MySettings);

const int ledPin = 13;      // select the pin for the LED

//Inputs
const int wiper        = 0;   //Position of fader relative to GND (Analog 0)

//Variables
const int   faderMax        = 1023;     //Value read by fader's maximum position (0-1023)
const int   faderMin        = 0;     //Value read by fader's minimum position (0-1023)
const int   faderChannel    = 1;     //Value from 1-8

void setup() {
  // declare the ledPin as an OUTPUT:
  pinMode(ledPin, OUTPUT);

  MIDI.begin();          // Launch MIDI and listen to channel 4

  Serial.begin(115200);
}

void loop()
{
  unsigned int vel = faderPosition();
//  updateFaderMidi(vel);
  // read the value from the sensor:

  // turn the ledPin on
  digitalWrite(ledPin, HIGH);  
  // stop the program for <sensorValue> milliseconds:
  updateFaderMidi(vel);
  delay(vel>>6);
  // turn the ledPin off:        
  digitalWrite(ledPin, LOW);   
  // stop the program for for <sensorValue> milliseconds:
  delay(vel>>6);
}

//Returns a MIDI pitch bend value for the fader's current position
//Cases ensure that there is a -infinity (min) and max value despite possible math error
unsigned int faderPosition()
{
  int position = analogRead(wiper);
  int returnValue = 0;
  
  if (position <= faderMin)
    returnValue = 0;
  else if (position >= faderMax)
    returnValue = 16383;
  else
    returnValue = ((long int)(position-faderMin)*16383) / (faderMax-faderMin);

  return returnValue;
}

void updateFaderMidi( int velocity )
{
  MIDI.sendPitchBend(velocity - 8192, faderChannel);
}
