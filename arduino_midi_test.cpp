#include <Arduino.h>
#include "/home/jet/tmp/arduino_midi_test/MIDI/MIDI.h"

void do_fader();

int main(void)
{
    init();

#if defined(USBCON)
    USBDevice.attach();
#endif
    setup();

    for (;;)
    {
        loop();
        if (serialEventRun) serialEventRun();
    }

    return 0;
}

struct MySettings : public midi::DefaultSettings
{
    static const bool UseRunningStatus = false; // Messes with my old equipment!
};

MIDI_CREATE_CUSTOM_INSTANCE(HardwareSerial, Serial, MIDI, MySettings);

// Pins
const int ledPin = 13;          // Select the pin for the LED
const int fader_analog_pin  = 0;           // Position of fader relative to GND (Analog 0)

// Variables
const int fader_max          = 1023;  // Value read by fader's maximum position (0-1023)
const int fader_min          = 0;     // Value read by fader's minimum position (0-1023)
const int fader_midi_channel = 1;     // Value from 1-16

void setup()
{
    // Declare the ledPin as an OUTPUT:
    pinMode(ledPin, OUTPUT);

    // Launch MIDI
    MIDI.begin();

    Serial.begin(115200);
}

void loop()
{
    // Read the value from the sensor:
    do_fader();

    // Turn the ledPin on for vel>>6 milliseconds
    digitalWrite(ledPin, HIGH);
    delay(250);

    // Turn the ledPin off for vel>>6 milliseconds
    digitalWrite(ledPin, LOW);
    delay(250);
}

// Returns a MIDI pitch bend value for the fader's current position
// Cases ensure that there is a -infinity (min) and max value despite possible math error
void do_fader()
{
    // Analog input position, and corresponding MIDI position
    int analog_pos = analogRead(fader_analog_pin);
    int midi_pos = 0;

    if (analog_pos <= fader_min)
        midi_pos = 0;
    else if (analog_pos >= fader_max)
        midi_pos = 16383;
    else
        midi_pos = ((long int)(analog_pos-fader_min)*16383) / (fader_max-fader_min);

    MIDI.sendPitchBend(midi_pos - 8192, fader_midi_channel);
}
