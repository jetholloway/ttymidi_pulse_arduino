#	Copyright 2019 Jet Holloway
#
#	This file is part of ttymidi_pulse_arduino.
#
#	ttymidi_pulse_arduino is free software: you can redistribute it and/or modify
#	it under the terms of the GNU General Public License as published by
#	the Free Software Foundation, either version 3 of the License, or
#	(at your option) any later version.
#
#	ttymidi_pulse_arduino is distributed in the hope that it will be useful,
#	but WITHOUT ANY WARRANTY; without even the implied warranty of
#	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#	GNU General Public License for more details.
#
#	You should have received a copy of the GNU General Public License
#	along with ttymidi_pulse_arduino.  If not, see <http://www.gnu.org/licenses/>.

ARDUINO_DIR = /usr/share/arduino

BOARD_TAG    = diecimila
ARDUINO_PORT = /dev/arduino

ARDUINO_LIBS =

OPTIMIZATION_LEVEL = s

CXX_FLAGS = -Wall -Wcast-align -Wconversion -Wextra -Wfloat-equal -Winit-self -Winline -Wmissing-declarations -Wmissing-include-dirs -Wno-long-long -Wpointer-arith -Wredundant-decls -Wshadow -Wswitch-default -Wswitch-enum -Wundef -Wuninitialized -Wunreachable-code -Wwrite-strings -Wzero-as-null-pointer-constant

include /usr/share/arduino/Arduino.mk
