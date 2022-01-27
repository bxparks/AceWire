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

#ifndef ACE_WIRE_SEEED_WIRE_INTERFACE_H
#define ACE_WIRE_SEEED_WIRE_INTERFACE_H

#include <stdint.h>

namespace ace_wire {

/**
 * A thin wrapper for the SoftwareI2C class from the
 * https://github.com/Seeed-Studio/Arduino_Software_I2C project so that it
 * becomes compatible with the AceWire API. The Arduino_Software_I2C library
 * uses no RX or TX buffer.
 *
 * @tparam T_WIRE underlying class which will be SoftwareI2C
 */
template <typename T_WIRE>
class SeeedWireInterface {
  public:
    /**
     * Constructor.
     * @param wire instance of `SoftwareI2C`
     */
    explicit SeeedWireInterface(T_WIRE& wire) : mWire(wire) {}

    /** Initial the interface. Currently does nothing. */
    void begin() const {}

    /** End the interface. Currently does nothing. */
    void end() const {}

    /**
     * Send the I2C address on the bus immediately since the underlying
     * SoftwareI2C class does not use a TX buffer.
     *
     * The underlying SoftwareI2C::beginTransmission() method returns a 0 if the
     * device responded with a NACK, and a 1 if ACK. This is opposite of the
     * convention used by the endTransmission() function. The AceWire library
     * defines the return value of beginTransmission() to use the same return
     * value convention as endTransmission(). So we have to flip the return
     * status from the Seeed library.
     *
     * @return returns 0 upon ACK from the device, 1 upon NACK from the device
     */
    uint8_t beginTransmission(uint8_t addr) const {
      return mWire.beginTransmission(addr) ^ 0x1;
    }

    /**
     * Write data immediately into the I2C bus with the Write bit set, since
     * the SoftwareI2C library does not use a TX buffer.
     *
     * @returns the number of bytes written into buffer if the device responded
     * with an ACK (will always be 1), or 0 if the device responded with a NACK
     */
    uint8_t write(uint8_t data) const {
      return (uint8_t) mWire.write(data);
    }

    /**
     * Send the data in the buffer. The sendStop parameter is ignored by the
     * SoftwareI2C implementation and the underlying implementation always sends
     * the STOP condition.
     *
     * Returns the value returned by the underlying
     * SoftwareI2C::endTransmission() method, which always returns 0 to indicate
     * success.
     */
    uint8_t endTransmission(bool sendStop = true) const {
      (void) sendStop;
      return mWire.endTransmission();
    }

    /**
     * Prepare to send quantity bytes to the device at addr. SoftwareI2C does
     * not implement at TX buffer, so the addr is sent immediately on the bus.
     *
     * @param addr I2C address
     * @param quantity number of bytes to read
     * @param sendStop whether to send a STOP condition after reading. This
     *    parameter is ignored by the SoftwareI2C class which always sends a
     *    STOP condition.
     *
     * @return the value returned by the underlying SoftwareI2C::requestFrom()
     * method, which is 0 for success if the device responded with an ACK, and 1
     * if the device sent a NACK
     */
    uint8_t requestFrom(uint8_t addr, uint8_t quantity, bool sendStop = true)
        const {
      (void) sendStop;
      return mWire.requestFrom(addr, quantity);
    }

    /**
     * Read byte from the I2C bus. Automatically sends a STOP condition after
     * reading quantity bytes as specified by requestFrom(). The `sendStop`
     * parameter is ignored.
     */
    uint8_t read() const {
      return mWire.read();
    }

    // Use default copy constructor and assignment operator.
    SeeedWireInterface(const SeeedWireInterface&) = default;
    SeeedWireInterface& operator=(const SeeedWireInterface&) = default;

  private:
    T_WIRE& mWire;
};

}

#endif
