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

#ifndef ACE_WIRE_TODBOT_WIRE_INTERFACE_H
#define ACE_WIRE_TODBOT_WIRE_INTERFACE_H

#include <stdint.h>

namespace ace_wire {

/**
 * A thin wrapper for the SoftI2CMaster class from the
 * https://github.com/todbot/SoftI2CMaster project so that it is compatible with
 * the AceWire API. The SoftI2CMaster library uses no RX or TX buffer.
 *
 * The `SoftI2CMaster` class name conflicts with the same class name in the
 * https://github.com/felias-fogg/SoftI2CMaster project, so the two cannot be
 * activated at the same time.
 *
 * @tparam T_WIRE underlying class which will be SoftI2CMaster
 */
template <typename T_WIRE>
class TodbotWireInterface {
  public:
    /**
     * Constructor.
     * @param wire instance of `SoftI2CMaster`
     */
    explicit TodbotWireInterface(T_WIRE& wire) : mWire(wire) {}

    /** Initial the interface. Currently does nothing. */
    void begin() const {}

    /** End the interface. Currently does nothing. */
    void end() const {}

    /**
     * Send the I2C address on the bus immediately, since the underlying
     * implementation does not use a TX buffer.
     *
     * The underlying SoftI2CMaster::beginTransmission() method returns a 0 if
     * the device responded with a NACK, and a 1 if ACK. This is opposite of the
     * convention used by the endTransmission() function. The AceWire library
     * defines the return value of beginTransmission() to use the same return
     * value convention as endTransmission(). So we have to flip the return
     * status from the SoftI2CMaster library.
     *
     * @return returns 0 upon ACK from the device, 1 upon NACK from the device
     */
    uint8_t beginTransmission(uint8_t addr) const {
      return mWire.beginTransmission(addr) ^ 0x1;
    }

    /**
     * Write data immediately into the I2C bus with the Write bit set, since
     * SoftI2CMaster library does not use a TX buffer.
     *
     * @returns the number of bytes written into buffer. It will be 1 if the
     * device responded with an ACK, it will be 0 if the device responds with a
     * NACK.
     */
    uint8_t write(uint8_t data) const {
      return (uint8_t) mWire.write(data);
    }

    /**
     * Send the data in the buffer. The sendStop parameter is ignored by the
     * SoftI2CMaster class and the underlying implementation always
     * sends the STOP condition.
     *
     * Returns the status value of the original beginTransmission().
     */
    uint8_t endTransmission(bool sendStop = true) const {
      (void) sendStop;
      return mWire.endTransmission();
    }

    /**
     * Prepare to send quantity bytes to the device at addr. SoftI2CMaster does
     * not implement at TX buffer, so the addr is sent immediately on the bus.
     *
     * @param addr I2C address
     * @param quantity number of bytes to read
     * @param sendStop whether to send a STOP condition after reading. This
     * parameter is ignored by the SoftI2CMaster implementation which *never*
     * sends a STOP condition
     *
     * @return the value returned by the underlying SoftI2CMaster::requestFrom()
     * method, which is 0 for success if the device responded with an ACK, and 1
     * if the device sent a NACK
     */
    uint8_t requestFrom(uint8_t addr, uint8_t quantity, bool sendStop = true)
        const {
      (void) sendStop; // sendStop not implemented by SoftI2CMaster
      return mWire.requestFrom(addr, quantity);
    }

    /**
     * Read byte from the I2C bus. The SoftI2CMaster library provides 2
     * different `read()` methods:
     *
     *  * `read()` for all bytes except the last one (which sends an ACK from
     *  the master to device), exposed by this method
     *  * `readLast()` to read the last byte (which sends a NACK from master to
     *  device), not exposed by this wrapper class
     *
     * It is probably possible for this adapter class to merge the two read()
     * methods, and generate the required STOP condition, so that it becomes
     * consistent with other I2C libraries. But I don't want to spend the effort
     * to do that because I don't use the SoftI2CMaster library.
     */
    uint8_t read() const {
      return mWire.read();
    }

    // Use default copy constructor and assignment operator.
    TodbotWireInterface(const TodbotWireInterface&) = default;
    TodbotWireInterface& operator=(const TodbotWireInterface&) = default;

  private:
    T_WIRE& mWire;
};

}

#endif
