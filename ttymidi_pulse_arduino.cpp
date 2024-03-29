/*
	Copyright 2019 Jet Holloway

	This file is part of ttymidi_pulse_arduino.

	ttymidi_pulse_arduino is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	ttymidi_pulse_arduino is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with ttymidi_pulse_arduino.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <Arduino.h>
#include "MIDI/MIDI.h"

struct MySettings : public midi::DefaultSettings
{
	static const bool UseRunningStatus = false; // Messes with my old equipment!
};

MIDI_CREATE_CUSTOM_INSTANCE(HardwareSerial, Serial, MIDI, MySettings);

// Constants
const int fader_max = 1023; // Value read by fader's maximum position (0-1023)
const int fader_min = 0;    // Value read by fader's minimum position (0-1023)

// Descriptions of Faders
const int fader_difference = 2; // Fader value must change by *more* than this
const unsigned int nr_faders   = 3;
const int fader_analog_pin[]   = {0,1,2};
const int fader_midi_channel[] = {1,2,3};
int fader_prev[]               = {0,0,0};

void do_fader( unsigned int fader_i );
void send_analogue_pos_over_MIDI( int MIDI_channel, int analog_pos );

//==============================================================================

int main(void)
{
	int loop_counter = 0;
	int ping_fader_i = 0;

	// Initialise the AVR chip
	init();

	// Something that was in the default arduino "main()" function
#if defined(USBCON)
	USBDevice.attach();
#endif

	// Initialize MIDI
	MIDI.begin();

	// Initialise serial port
	Serial.begin(38400);

	while ( true )
	{
		// Run main functionality for each fader
		for ( unsigned int i = 0; i < nr_faders; i++ )
			do_fader(i);

		// Every second, just send one of the fader values again
		if ( loop_counter % 100 == 0 )
		{
			send_analogue_pos_over_MIDI( fader_midi_channel[ping_fader_i], fader_prev[ping_fader_i]);

			// Cycle through all the faders
			ping_fader_i = (ping_fader_i + 1) % nr_faders;
		}

		// This is not needed.  Only if we want to receive over RS-232
		if ( serialEventRun )
			serialEventRun();

		loop_counter++;
		delay(10);
	}

	return 0;
}

void send_analogue_pos_over_MIDI( int MIDI_channel, int analog_pos )
{
	int midi_pos = 0;

	if (analog_pos <= fader_min)
		midi_pos = 0;
	else if (analog_pos >= fader_max)
		midi_pos = 16383;
	else
		midi_pos = ((long int)(analog_pos-fader_min)*16383) / (fader_max-fader_min);

	MIDI.sendPitchBend(midi_pos - 8192, MIDI_channel);
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

	// Only bother with it if the value has changed sufficiently
	if ( abs(analog_pos-fader_prev[fader_i]) > fader_difference )
	{
		// If the value changes by a lot, then just accept the new value
		if ( abs(analog_pos-fader_prev[fader_i]) > 2*fader_difference )
			fader_prev[fader_i] = analog_pos;
		else // If the new value is just outside the range of accepted values, then just tweak the prev value
		{
			if ( analog_pos > fader_prev[fader_i] )
				fader_prev[fader_i]++;
			if ( analog_pos < fader_prev[fader_i] )
				fader_prev[fader_i]--;
		}

		send_analogue_pos_over_MIDI( fader_midi_channel[fader_i], analog_pos );
	}
}
