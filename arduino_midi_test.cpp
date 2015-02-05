#include <Arduino.h>
#include "/home/jet/tmp/arduino_midi_test/MIDI/MIDI.h"

void do_fader( unsigned int fader_i );

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

// Variables
const int fader_max          = 1023;  // Value read by fader's maximum position (0-1023)
const int fader_min          = 0;     // Value read by fader's minimum position (0-1023)

// Descriptions of Faders
const int fader_difference = 1;         // Fader value must change by *more* than this
const unsigned int nr_faders = 2;
const int fader_analog_pin[]   = {0,1};
const int fader_midi_channel[] = {1,2};
int fader_prev[]               = {0,0};

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
    for ( unsigned int i = 0; i < nr_faders; i++ )
        do_fader(i);

    // Turn the ledPin on for vel>>6 milliseconds
    digitalWrite(ledPin, HIGH);
    delay(250);

    // Turn the ledPin off for vel>>6 milliseconds
    digitalWrite(ledPin, LOW);
    delay(250);
}

void do_fader( unsigned int fader_i )
{
    //   The Arduino only has one ADC shared between all channels.  This is an
    // extra read to flush the old value out of the sample-and-hold-buffer (or
    // however it works internally).  If you don't have this, the previously
    // read analogue value will affect this one.
    // I have tested it, and only one extra read is necessary
    analogRead(fader_analog_pin[fader_i]);

    // Analog input position, and corresponding MIDI position
    int analog_pos = analogRead(fader_analog_pin[fader_i]);
    int midi_pos = 0;

    // Only bother with it if the value has changed sufficiently
    if ( abs(analog_pos-fader_prev[fader_i]) > fader_difference )
    {
        // If the value changes loads, then just accept the new value
        if ( abs(analog_pos-fader_prev[fader_i]) > 2*fader_difference )
            fader_prev[fader_i] = analog_pos;
        else // If the new value is just outside the range of accepted values, then just tweak the prev value
        {
            if ( analog_pos > fader_prev[fader_i] )
                fader_prev[fader_i]++;
            if ( analog_pos < fader_prev[fader_i] )
                fader_prev[fader_i]--;
        }

        if (analog_pos <= fader_min)
            midi_pos = 0;
        else if (analog_pos >= fader_max)
            midi_pos = 16383;
        else
            midi_pos = ((long int)(analog_pos-fader_min)*16383) / (fader_max-fader_min);

        MIDI.sendPitchBend(midi_pos - 8192, fader_midi_channel[fader_i]);
    }
}
