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

#ifndef ACE_WIRE_THEXENO_WIRE_INTERFACE_H
#define ACE_WIRE_THEXENO_WIRE_INTERFACE_H

#include <stdint.h>

namespace ace_wire {

/**
 * A thin wrapper around the `TwoWire` class from the
 * https://github.com/thexeno/HardWire-Arduino-Library project so that it
 * becomes compatible with the AceWire API. The HardWire library uses a TX
 * buffer and an RX buffer of 32 bytes each.
 *
 * @tparam T_WIRE underlying I2C class which will always be `TwoWire`
 */
template <typename T_WIRE>
class ThexenoWireInterface {
  public:
    /**
     * Constructor.
     * @param wire instance of `HardWire`
     */
    explicit ThexenoWireInterface(T_WIRE& wire) : mWire(wire) {}

    /** Initialize the interface. Currently does nothing. */
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
     * @returns the number of bytes written into buffer, will always be 1.
     */
    uint8_t write(uint8_t data) const {
      return (uint8_t) mWire.write(data);
    }

    /**
     * Send the data in the buffer, with a STOP condition if `sendStop` is true.
     *
     * Returns the value returned by the underlying TwoWire::endTransmission()
     * method, which returns the values documented in the twi_writeTo()
     * function:
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
     * Read bytes from the slave and store in buffer owned by TwoWire and
     * send a STOP condition if `sendStop` is true.
     *
     * @param addr I2C address
     * @param quantity number of bytes to read
     * @param sendStop whether the STOP condition should be sent at end
     *
     * @return the value returned by the underlying T_WIRE::requestFrom()
     * method, which will normally be 'quantity'
     */
    uint8_t requestFrom(uint8_t addr, uint8_t quantity, bool sendStop = true)
        const {
      return mWire.requestFrom(addr, quantity, (uint8_t) sendStop);
    }

    /** Read byte from the TwoWire receive buffer. */
    uint8_t read() const {
      return mWire.read();
    }

    // Use default copy constructor and assignment operator.
    ThexenoWireInterface(const ThexenoWireInterface&) = default;
    ThexenoWireInterface& operator=(const ThexenoWireInterface&) = default;

  private:
    T_WIRE& mWire;
};

}

#endif
