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

/*
 * A sketch that writes and reads from a DS3231 using TwoWireInterace,
 * SimpleWireInterface, and SimpleWireFastInterface (on AVR processors).
 * Requires an actual DS3231 device on the I2C bus.
 *
 * Should print the following:
 *
 * AVR:
 *    runTwoWire()
 *    runSimpleWire()
 *    runSimpleWireFast()
 *    Done
 * ESP8266:
 *    runTwoWire()
 *    runSimpleWire()
 *    Done
 * ESP32
 *    runSimpleWire()
 *    runTwoWire()
 *    Done
 */

#include <Arduino.h>
#include <Wire.h> // TwoWire
#include <AceWire.h>
using ace_wire::TwoWireInterface;
using ace_wire::SimpleWireInterface;

#if defined(ARDUINO_ARCH_AVR) || defined(EPOXY_DUINO)
  #include <digitalWriteFast.h>
  #include <ace_wire/SimpleWireFastInterface.h>
  using ace_wire::SimpleWireFastInterface;
#endif

#if ! defined(SERIAL_PORT_MONITOR)
#define SERIAL_PORT_MONITOR Serial
#endif

//------------------------------------------------------------------
// I2C communication configs.
//------------------------------------------------------------------

const uint8_t SDA_PIN = SDA;
const uint8_t SCL_PIN = SCL;
const uint8_t DELAY_MICROS = 1;
const uint8_t DS3231_I2C_ADDRESS = 0x68;

//------------------------------------------------------------------

// Send data to DS3231 over I2C, checking for errors along the way.
// Total bytes sent: 9 bytes.
template <typename T_WIREI>
void sendData(T_WIREI& wireInterface) {
  // Send I2C address: 1 byte
  uint8_t res = wireInterface.beginTransmission(DS3231_I2C_ADDRESS);
  if (res) SERIAL_PORT_MONITOR.println(F("Error: beginTransmission()"));

  // Send start at position 7: 1 byte
  res = wireInterface.write(0x07);
  if (res == 0) SERIAL_PORT_MONITOR.println(F("Error: write(command)"));

  // Send 7 bytes to the alarm1 and alarm2 registers. Using the alarm registers
  // avoids changing the date and time stored on the DS3231 in case it is
  // actually being used as an active date/time source.
  res = wireInterface.write(0x01); // alarm1 seconds
  if (res != 1) SERIAL_PORT_MONITOR.println(F("Error: write()"));
  res = wireInterface.write(0x02); // alarm1 minutes
  if (res != 1) SERIAL_PORT_MONITOR.println(F("Error: write()"));
  res = wireInterface.write(0x03); // alarm1 hours
  if (res != 1) SERIAL_PORT_MONITOR.println(F("Error: write()"));
  res = wireInterface.write(0x04); // alarm1 day/date
  if (res != 1) SERIAL_PORT_MONITOR.println(F("Error: write()"));
  res = wireInterface.write(0x05); // alarm2 minutes
  if (res != 1) SERIAL_PORT_MONITOR.println(F("Error: write()"));
  res = wireInterface.write(0x06); // alarm2 hours
  if (res != 1) SERIAL_PORT_MONITOR.println(F("Error: write()"));
  res = wireInterface.write(0x07); // alarm2 day/date
  if (res != 1) SERIAL_PORT_MONITOR.println(F("Error: write()"));

  // Send the STOP condition.
  res = wireInterface.endTransmission();
  if (res) SERIAL_PORT_MONITOR.println(F("Error: endTransmission()"));
}

// Read data from DS3231 over I2C. Read from the alarm1 and alarm2 registers
// that was written by sendData().
template <typename T_WIREI>
void readData(T_WIREI& wireInterface) {
  // Set start at position 7: 1 byte
  uint8_t res = wireInterface.beginTransmission(DS3231_I2C_ADDRESS);
  if (res) SERIAL_PORT_MONITOR.println(F("Error: beginTransmission()"));
  res = wireInterface.write(0x07);
  if (res == 0) SERIAL_PORT_MONITOR.println(F("Error: write(command)"));
  res = wireInterface.endTransmission();
  if (res) SERIAL_PORT_MONITOR.println(F("Error: endTransmission()"));

  // Request from I2C address: 1 byte
  res = wireInterface.requestFrom(
      DS3231_I2C_ADDRESS, 7 /*quantity*/, true /*sendStop*/);
  if (res != 7) SERIAL_PORT_MONITOR.println(F("Error: requestFrom()"));

  // Read the 7 bytes from the alarm registers that we set in sendData(). The
  // slave device has no ability to send an error condition to the master, so we
  // must always read the 9 bytes specified in requestFrom().
  res = wireInterface.read(); // alarm1 seconds
  if (res != 0x01) SERIAL_PORT_MONITOR.println(F("Error: read(1)"));
  res = wireInterface.read(); // alarm1 minutes
  if (res != 0x02) SERIAL_PORT_MONITOR.println(F("Error: read(2)"));
  res = wireInterface.read(); // alarm1 hours
  if (res != 0x03) SERIAL_PORT_MONITOR.println(F("Error: read(3)"));
  res = wireInterface.read(); // alarm1 day/date
  if (res != 0x04) SERIAL_PORT_MONITOR.println(F("Error: read(4)"));
  res = wireInterface.read(); // alarm2 minutes
  if (res != 0x05) SERIAL_PORT_MONITOR.println(F("Error: read(5)"));
  res = wireInterface.read(); // alarm2 hours
  if (res != 0x06) SERIAL_PORT_MONITOR.println(F("Error: read(6)"));
  res = wireInterface.read(); // alarm2 day/date
  if (res != 0x07) SERIAL_PORT_MONITOR.println(F("Error: read(7)"));

  // The last read() above automatically sends the STOP condition to the slave.
}

void runTwoWire() {
  SERIAL_PORT_MONITOR.println(F("runTwoWire()"));

  using WireInterface = TwoWireInterface<TwoWire>;
  WireInterface wireInterface(Wire);

  Wire.begin();
  wireInterface.begin();
  sendData(wireInterface);
  readData(wireInterface);
  wireInterface.end();

#if ! defined(ESP8266)
  Wire.end();
#endif
}

void runSimpleWire() {
  SERIAL_PORT_MONITOR.println(F("runSimpleWire()"));

  using WireInterface = SimpleWireInterface;
  WireInterface wireInterface(SDA_PIN, SCL_PIN, DELAY_MICROS);

  wireInterface.begin();
  sendData(wireInterface);
  readData(wireInterface);
  wireInterface.end();
}

#if defined(ARDUINO_ARCH_AVR) || defined(EPOXY_DUINO)
void runSimpleWireFast() {
  SERIAL_PORT_MONITOR.println(F("runSimpleWireFast()"));

  using WireInterface = SimpleWireFastInterface<SDA_PIN, SCL_PIN, DELAY_MICROS>;
  WireInterface wireInterface;

  wireInterface.begin();
  sendData(wireInterface);
  readData(wireInterface);
  wireInterface.end();
}
#endif

//-----------------------------------------------------------------------------

void setup() {
#if ! defined(EPOXY_DUINO)
  delay(1000); // Wait for stability on some boards, otherwise garage on Serial
#endif

  SERIAL_PORT_MONITOR.begin(115200);
  while (!SERIAL_PORT_MONITOR); // Wait for Leonardo/Micro

#if ! defined(ESP32)
  // Run TwoWire at the end on ESP32, because it prevents any other I2C library
  // from working on those pins once Wire setup.
  runTwoWire();
  yield();
#endif

  runSimpleWire();
  yield();

#if defined(ARDUINO_ARCH_AVR) || defined(EPOXY_DUINO)
  runSimpleWireFast();
  yield();
#endif

#if defined(ESP32)
  runTwoWire();
  yield();
#endif

  SERIAL_PORT_MONITOR.println(F("Done"));

#if defined(EPOXY_DUINO)
  exit(0);
#endif
}

void loop() {}
