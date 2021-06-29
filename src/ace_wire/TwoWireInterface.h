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

#ifndef ACE_WIRE_TWO_WIRE_INTERFACE_H
#define ACE_WIRE_TWO_WIRE_INTERFACE_H

#include <stdint.h>

namespace ace_wire {

/**
 * A thin wrapper around an I2C `TwoWire` class and its `Wire` object. This is a
 * template class to avoid including the <Wire.h> header file, which increases
 * flash memory on AVR by about 1000 byte even if the Wire object is never used.
 *
 * This wrapper can also be used with alternative implementations of I2C
 * (software or hardware) so long as they implement some basic API of the
 * `TwoWire` class: specifically the `beginTransmission()`, `write()` and
 * `endTransmission() methods. Since `TwoWireInterface` is a template, the
 * alternative implementation classes do *not* need to inherit from the
 * `TwoWire` class.
 */
template <typename T_WIRE>
class TwoWireInterface {
  public:
    /**
     * Constructor.
     * @param wire instance of `T_WIRE`. If the pre-installed Wire.h is
     *    used, then T_WIRE is `TwoWire` and `wire` is the precreated `Wire`
     *    object
     */
    TwoWireInterface(T_WIRE& wire) : mWire(wire) {}

    /** Initial the interface. Currently does nothing. */
    void begin() {}

    /** End the interface. Currently does nothing. */
    void end() {}

    /** Start transmission at specified I2C addr. */
    void beginTransmission(uint8_t addr) {
      mWire.beginTransmission(addr);
    }

    /** Write data. */
    void write(uint8_t data) {
      mWire.write(data);
    }

    /** End transmission. */
    void endTransmission() {
      mWire.endTransmission();
    }

    // Use default copy constructor and assignment operator.
    TwoWireInterface(const TwoWireInterface&) = default;
    TwoWireInterface& operator=(const TwoWireInterface&) = default;

  private:
    T_WIRE& mWire;
};

}

#endif
