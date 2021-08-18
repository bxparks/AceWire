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

#ifndef ACE_WIRE_SIMPLE_WIRE_FAST_INTERFACE_H
#define ACE_WIRE_SIMPLE_WIRE_FAST_INTERFACE_H

#include <stdint.h>
#include <Arduino.h> // delayMicroseconds()

namespace ace_wire {

/**
 * A version of SimpleWireInterface that uses one of the <digitalWriteFast.h>
 * libraries. The biggest benefit of using digitalWriteFast is the reduction of
 * flash memory size, 500-700 bytes on AVR.
 *
 * The `delayMicroseconds()` may not be accurate for small values on some
 * processors (e.g. AVR) . The actual minimum value of T_DELAY_MICROS will
 * depend on the capacitance and resistance on the DATA and CLOCK lines, and the
 * accuracy of the `delayMicroseconds()` function.
 *
 * @tparam T_DATA_PIN SDA pin
 * @tparam T_CLOCK_PIN SCL pin
 * @tparam T_DELAY_MICROS delay after each bit transition of SDA or SCL
 */
template <
    uint8_t T_DATA_PIN,
    uint8_t T_CLOCK_PIN,
    uint8_t T_DELAY_MICROS
>
class SimpleWireFastInterface {
  public:
    /** Constructor. */
    explicit SimpleWireFastInterface() = default;

    /**
     * Initialize the clock and data pins.
     *
     * These are open-drain lines, with pull-up resistors. We must not drive
     * them HIGH actively since that could damage the transitor at the other
     * end of the line pulling LOW. Instead, we go into INPUT mode to let the
     * line to HIGH through the pullup resistor, then go to OUTPUT mode only
     * to pull down.
     */
    void begin() const {
      digitalWriteFast(T_CLOCK_PIN, LOW);
      digitalWriteFast(T_DATA_PIN, LOW);

      // Begin with both lines in INPUT mode to passively go HIGH.
      clockHigh();
      dataHigh();
    }

    /** Set clock and data pins to INPUT mode. */
    void end() const {
      clockHigh();
      dataHigh();
    }

    /**
     * Send I2C START condition.
     *
     * @param addr I2C address of slave device
     * @return 0 if ACK, 1 if NACK
     */
    uint8_t beginTransmission(uint8_t addr) const {
      clockHigh();
      dataHigh();

      dataLow();
      clockLow();

      // Send I2C addr (7 bits) and the R/W bit set to "write" (0x00).
      uint8_t effectiveAddr = (addr << 1) | 0x00;
      uint8_t res = write(effectiveAddr);
      return res ^ 0x1;
    }

    /**
     * Send the data byte on the data bus, with MSB first as specified by I2C.
     *
     * This loop generates slightly asymmetric logic signals because clockLow()
     * lasts for 2*bitDelay(), but clockHigh() lasts for only 1*bitDelay(). This
     * does not seem to cause any problems with the LED modules that I have
     * tested.
     *
     * @return 1 if successful with ACK, 0 for NACK.
     */
    uint8_t write(uint8_t data) const {
      for (uint8_t i = 0;  i < 8; ++i) {
        if (data & 0x80) {
          dataHigh();
        } else {
          dataLow();
        }
        clockHigh();
        // An extra bitDelay() here would make the HIGH and LOW states symmetric
        // in duration (if digitalWriteFast() is assumed to be infinitely fast).
        // But actual devices that I have tested seem to support the absence of
        // that extra delay. So let's ignore it to make the transfer speed
        // faster.
        clockLow();
        data <<= 1;
      }

      uint8_t ack = readAck();
      return ack ^ 0x1;
    }

    /**
     * Send the I2C STOP condition.
     *
     * @return always returns 0 to indicate success
     */
    uint8_t endTransmission(bool sendStop = true) const {
      // clock will always be LOW when this is called
      if (sendStop) {
        dataLow();
        clockHigh();
        dataHigh();
      }

      return 0;
    }

    /**
     * Prepare to read bytes by sending I2C START condition. If `sendStop` is
     * true, then a STOP condition will be sent by `read()` after the last byte.
     *
     * @return 'quantity' if addr was written successfully and the device
     * responded with ACK, 0 if device responded with NACK
     */
    uint8_t requestFrom(
        uint8_t addr, uint8_t quantity, bool sendStop = true) const {
      mQuantity = quantity;
      mSendStop = sendStop;

      clockHigh();
      dataHigh();

      dataLow();
      clockLow();

      // Send I2C addr (7 bits) and the R/W bit set to "read" (0x01).
      uint8_t effectiveAddr = (addr << 1) | 0x01;
      uint8_t status = write(effectiveAddr);

      return (status == 0) ? 0 : quantity;
    }

    /**
     * Read byte. After reading 8 bits, an ACK or NACK will be sent by the
     * master to the slave. ACK means the slave will be asked to send more
     * bytes so can hold control of the data line. NACK means no more bytes will
     * be read from the slave and the slave should release the data line.
     *
     * If requestFrom() was called with `sendStop = true`, a STOP condition
     * will be sent after reading the final byte.
     *
     * If called when the number of remaining bytes is 0 (which should not
     * happen if the calling program is correctly implemented), this method
     * returns immediately with a 0xff.
     */
    uint8_t read() const {
      // Caller should not call when mQuantity is 0, but let's guard against it.
      if (! mQuantity) return 0xff;

      // Read one byte
      dataHigh();
      uint8_t data = 0;
      for (uint8_t i = 0; i < 8; ++i) {
        clockHigh();
        data <<= 1;
        uint8_t bit = digitalReadFast(T_DATA_PIN);
        data |= (bit & 0x1);
        clockLow();
      }

      // Decrement quantity to determine if NACK or ACK should be sent.
      mQuantity--;
      if (mQuantity) {
        sendAck();
      } else {
        sendNack();
        endTransmission(mSendStop);
      }

      return data;
    }

    // Use default copy constructor and assignment operator.
    SimpleWireFastInterface(const SimpleWireFastInterface&) = default;
    SimpleWireFastInterface& operator=(const SimpleWireFastInterface&) =
        default;

  private:
    /**
     * Read the ACK/NACK bit from the device which is expected to be set after
     * the falling edge of the 8th CLK, which happens in the write() loop above.
     *
     * @return 0 for ACK (active LOW), 1 or NACK (passive HIGH).
     */
    static uint8_t readAck() {
      // Go into INPUT mode, reusing dataHigh(), saving 10 flash bytes on AVR.
      dataHigh();

      // Set the clock HIGH, because the I2C protocol says that SDA will not
      // change when SCL is HIGH and we expect the slave to abide by that.
      clockHigh();

      uint8_t ack = digitalReadFast(T_DATA_PIN);

      // Device releases SDA upon falling edge of the 9th CLK.
      clockLow();
      return ack;
    }

    /** Send ACK to slave. */
    static void sendAck() {
      dataLow();
      clockHigh();
      clockLow();
    }

    /** Send NACK to slave. */
    static void sendNack() {
      dataHigh();
      clockHigh();
      clockLow();
    }

    static void bitDelay() { delayMicroseconds(T_DELAY_MICROS); }

    static void clockHigh() { pinModeFast(T_CLOCK_PIN, INPUT); bitDelay(); }

    static void clockLow() { pinModeFast(T_CLOCK_PIN, OUTPUT); bitDelay(); }

    static void dataHigh() { pinModeFast(T_DATA_PIN, INPUT); bitDelay(); }

    static void dataLow() { pinModeFast(T_DATA_PIN, OUTPUT); bitDelay(); }

  private:
    mutable bool mSendStop;
    mutable uint8_t mQuantity;
};

}

#endif
