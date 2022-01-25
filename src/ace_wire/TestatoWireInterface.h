/*
MIT License

Copyright (c) 2022 Brian T. Park

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

#ifndef ACE_WIRE_TESTATO_WIRE_INTERFACE_H
#define ACE_WIRE_TESTATO_WIRE_INTERFACE_H

#include <stdint.h>

namespace ace_wire {

/**
 * A thin wrapper around the `SoftwareWire` class provided by the
 * https://github.com/Testato/SoftwareWire project so that it becomes compatible
 * with the AceWire interface.
 *
 * @tparam T_WIRE the template parameter for the I2C class which will always be
 * `SoftwareWire`
 */
template <typename T_WIRE>
class TestatoWireInterface {
  public:
    /**
     * Constructor.
     * @param wire instance of `SoftwareWire`
     */
    explicit TestatoWireInterface(T_WIRE& wire) : mWire(wire) {}

    /** Initial the interface. Currently does nothing. */
    void begin() const {}

    /** End the interface. Currently does nothing. */
    void end() const {}

    /**
     * Prepare the write buffer to accept a sequence of data, and save the addr
     * for transmission when `endTransmission()` is called. SoftwareWire
     * uses no TX  buffer, so this method *should* return the ACK/NACK response
     * from the device, but it actually returns void, so the only thing we can
     * do is always return a 0 to indicate success.
     *
     * @return always returns 0 to indicate success because SoftwareWire does
     * not have a TX buffer
     */
    uint8_t beginTransmission(uint8_t addr) const {
      mWire.beginTransmission(addr);
      return 0;
    }

    /**
     * Write data to the I2C bus. SoftwareWire uses *no* TX buffer, so this is
     * written directly to the bus. SoftwareWire::write() returns a `size_t`,
     * which is converted to a `uint8_t` for compatibility with AceWire.
     *
     * @returns 1 always to indicate success, since SoftwareWire::write() always
     *    returns 1
     */
    uint8_t write(uint8_t data) const {
      return (uint8_t) mWire.write(data);
    }

    /**
     * Send a STOP condition if `sendStop` is true, or a REPEATED_START
     * condition if `sendStop` is false.
     *
     * Returns a status value to indicate the success or failure of the previous
     * write() statements.
     *
     *  * 0: success
     *  * 1: length too long for buffer
     *  * 2: address send, NACK received
     *  * 3: data send, NACK received
     *  * 4: other twi error (lost bus arbitration, bus error, ..)
     */
    uint8_t endTransmission(bool sendStop = true) const {
      return mWire.endTransmission(sendStop);
    }

    /**
     * Prepare to read bytes from the device at the given address, and
     * send a STOP condition if `sendStop` is true. The underlying SoftwareWire
     * does not use a TX buffer, so the addr is immediately placed on the I2C
     * bus.
     *
     * @return `quantity` if the device responded with an ACK, or 0 if the
     * device responded with a NACK.
     */
    uint8_t requestFrom(uint8_t addr, uint8_t quantity, bool sendStop = true)
        const {
      return mWire.requestFrom(addr, quantity, sendStop);
    }

    /** Read byte from buffer. */
    uint8_t read() const {
      return mWire.read();
    }

    // Use default copy constructor and assignment operator.
    TestatoWireInterface(const TestatoWireInterface&) = default;
    TestatoWireInterface& operator=(const TestatoWireInterface&) = default;

  private:
    T_WIRE& mWire;
};

}

#endif
