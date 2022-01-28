/*
MIT License

Copyright (c) 2021 Brian T. Park

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/**
 * @mainpage AceWire Library
 *
 * This is the Doxygen documentation for the
 * <a href="https://github.com/bxparks/AceWire">AceWire Library</a>.
 *
 * Click on the "Classes" menu above to see the list of classes.
 *
 * Click on the "Files" menu above to see the list of header files.
 */

#ifndef ACE_WIRE_H
#define ACE_WIRE_H

// Version format: xxyyzz == "xx.yy.zz"
#define ACE_WIRE_VERSION 401
#define ACE_WIRE_VERSION_STRING "0.4.1"

// Blacklist platforms using https://github.com/arduino/ArduinoCore-api due to
// incompatibilities.
#if defined(ARDUINO_API_VERSION)
#error Platforms using ArduinoCore-API not supported
#endif

// Files exported by this main header file.

// Implementations provided by this library.
#include "ace_wire/SimpleWireInterface.h"

// The following commented out because it works only on AVR platforms with a
// suitable <digitalWriteFast.h> library. End-user should include this header
// file manually, right after the `#include <AceWire.h>`.
//#include "ace_wire/SimpleWireFastInterface.h"

// Wrapper around pre-installed <Wire.h>.
#include "ace_wire/TwoWireInterface.h"

// Implementations for all platforms.
#include "ace_wire/FeliasFoggWireInterface.h"
#include "ace_wire/MarpleWireInterface.h"
#include "ace_wire/RaemondWireInterface.h"
#include "ace_wire/SeeedWireInterface.h"

// Implementations for AVR only.
#include "ace_wire/TestatoWireInterface.h"
#include "ace_wire/ThexenoWireInterface.h"
#include "ace_wire/TodbotWireInterface.h"

#endif
