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

#ifndef ACE_WIRE_FELIAS_FOGG_WIRE_INTERFACE_H
#define ACE_WIRE_FELIAS_FOGG_WIRE_INTERFACE_H

#include <stdint.h>

namespace ace_wire {

/**
 * A thin wrapper around the `SlowSoftWire` class provided by the
 * https://github.com/felias-fogg/SlowSoftWire project so that it becomes
 * compatible with the AceWire API. The SlowSoftWire class uses a 32-byte RX
 * buffer and a 32-byte TX buffer.
 *
 * @tparam T_WIRE underlying I2C class which will always be `SlowSoftWire`
 */
template <typename T_WIRE>
class FeliasFoggWireInterface {
  public:
    /**
     * Constructor.
     * @param wire instance of `SlowSoftWire`
     */
    explicit FeliasFoggWireInterface(T_WIRE& wire) : mWire(wire) {}

    /** Initial the interface. Currently does nothing. */
    void begin() const {}

    /** End the interface. Currently does nothing. */
    void end() const {}

    /**
     * Prepare the write buffer to accept a sequence of data, and save the addr
     * for transmission when `endTransmission()` is called.
     *
     * @return always returns 0 to indicate success because the `addr` is simply
     *    written into a buffer
     */
    uint8_t beginTransmission(uint8_t addr) const {
      mWire.beginTransmission(addr);
      return 0;
    }

    /**
     * Write data into the write buffer.
     *
     * @returns the number of bytes written into buffer, which will always be 1
     */
    uint8_t write(uint8_t data) const {
      return (uint8_t) mWire.write(data);
    }

    /**
     * Send the data in the buffer, and return the following status code:
     *
     *  * 0: success
     *  * 1: length too long for buffer (not used by SlowSoftWire)
     *  * 2: address send, NACK received
     *  * 3: data send, NACK received
     *  * 4: other twi error (lost bus arbitration, bus error, ..) (not used by
     *       SlowSoftWire)
     *
     * @param sendStop controls whether or not a STOP condition is sent at the
     * end of the buffer
     */
    uint8_t endTransmission(bool sendStop = true) const {
      return mWire.endTransmission(sendStop);
    }

    /**
     * Read bytes from the slave and store in buffer owned by SoftWire
     *
     * @param addr I2C address
     * @param quantity number of bytes to read from I2C bus
     * @param sendStop controls whether the STOP condition should be sent
     *
     * @return the value returned by the underlying SoftWire::requestFrom()
     * method, which will normally be 'quantity'.
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
    FeliasFoggWireInterface(const FeliasFoggWireInterface&) = default;
    FeliasFoggWireInterface& operator=(
        const FeliasFoggWireInterface&) = default;

  private:
    T_WIRE& mWire;
};

}

#endif
