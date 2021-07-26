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
    explicit TwoWireInterface(T_WIRE& wire) : mWire(wire) {}

    /** Initial the interface. Currently does nothing. */
    void begin() {}

    /** End the interface. Currently does nothing. */
    void end() {}

    /**
     * For buffered implementations, prepare the write buffer to accept a
     * sequence of data, and save the addr for transmission when
     * `endTransmission()` is called. For unbuffered implementations,
     * immediately send the address byte on the I2C bus with the Write bit set.
     */
    void beginTransmission(uint8_t addr) {
      mWire.beginTransmission(addr);
    }

    /**
     * For buffered implementations, Write data into the write buffer. For
     * unbuffered implementations, actually send the byte to the I2C device.
     *
     * Returns the value returned by the underlying I2C T_WIRE::write() method.
     * Usually, a 1 is returned if the data was written successfully, 0
     * otherwise.
     */
    uint8_t write(uint8_t data) {
      return mWire.write(data);
    }

    /**
     * For buffered implementations, end building of the buffer, and actually
     * transmit the data. For unbuffered implementaitons, send the STOP
     * condition if `sendStop` is true, otherwise do nothing.
     *
     * Returns the value returned by the underlying T_WIRE::endTransmission()
     * method. For the preinstalled Wire library, the status value definitions
     * are buried in the twi_writeTo() function:
     *
     *  * 0: success
     *  * 1: length to long for buffer
     *  * 2: address send, NACK received
     *  * 3: data send, NACK received
     *  * 4: other twi error (lost bus arbitration, bus error, ..)
     */
    uint8_t endTransmission(bool sendStop) {
      return mWire.endTransmission(sendStop);
    }

    /**
     * Same as endTransmission(bool) but always send STOP condition. Some
     * implementaitons do not support repeated START, so do not provide an
     * `endTransmission(bool)` method that takes a bool.
     */
    uint8_t endTransmission() {
      return mWire.endTransmission();
    }

    /**
     * Read bytes from the slave and store in buffer owned by T_WIRE and
     * send a STOP condition if `sendStop` is true.
     *
     * @return the value returned by the underlying T_WIRE::requestFrom()
     * method, which will normally be 'quantity'.
     */
    uint8_t requestFrom(uint8_t addr, uint8_t quantity, bool sendStop) {
      return mWire.requestFrom(addr, quantity, (uint8_t) sendStop);
    }

    /**
     * Read bytes from the slave and store in buffer owned by T_WIRE, and always
     * send a STOP condition.
     *
     * Some I2C implementations do not provide a 3-argument version of
     * requestFrom(), so we need to provide an explicit 2-argument versions
     * instead of using a default argument of `sendStop = true`.
     *
     * @return the value returned by the underlying T_WIRE::requestFrom()
     * method, which will normally be 'quantity' or 0 if an error is
     * encountered. Different T_WIRE implementations may handle error conditions
     * differently.
     */
    uint8_t requestFrom(uint8_t addr, uint8_t quantity) {
      return mWire.requestFrom(addr, quantity);
    }

    /** Read byte from buffer. */
    uint8_t read() {
      return mWire.read();
    }

    /**
     * End requestFrom(). Clients should always call this. This implementation
     * does nothing, but other implementations will perform additional actions.
     */
    void endRequest() {}

    // Use default copy constructor and assignment operator.
    TwoWireInterface(const TwoWireInterface&) = default;
    TwoWireInterface& operator=(const TwoWireInterface&) = default;

  private:
    T_WIRE& mWire;
};

}

#endif
