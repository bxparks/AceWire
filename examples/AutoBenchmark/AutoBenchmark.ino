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
 * A sketch that generates the min/avg/max duration (in microsecondes) of the
 * an I2C transaction using different implementations.
 */

#include <stdio.h>
#include <Arduino.h>
#include <Wire.h> // TwoWire
#include <AceCommon.h> // TimingStats
#include <AceWire.h>

#if defined(ARDUINO_ARCH_AVR) || defined(EPOXY_DUINO)
#include <digitalWriteFast.h>
#include <ace_wire/SimpleWireFastInterface.h>
#endif

using ace_wire::TwoWireInterface;
using ace_wire::SimpleWireInterface;
#if defined(ARDUINO_ARCH_AVR) || defined(EPOXY_DUINO)
using ace_wire::SimpleWireFastInterface;
#endif

using ace_common::TimingStats;

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
// Run benchmarks.
//------------------------------------------------------------------

/** Print the result for each LedMatrix algorithm. */
static void printStats(
    const __FlashStringHelper* name,
    const TimingStats& stats,
    uint16_t numSamples) {
  SERIAL_PORT_MONITOR.print(name);
  SERIAL_PORT_MONITOR.print(' ');
  SERIAL_PORT_MONITOR.print(stats.getMin());
  SERIAL_PORT_MONITOR.print(' ');
  SERIAL_PORT_MONITOR.print(stats.getAvg());
  SERIAL_PORT_MONITOR.print(' ');
  SERIAL_PORT_MONITOR.print(stats.getMax());
  SERIAL_PORT_MONITOR.print(' ');
  SERIAL_PORT_MONITOR.println(numSamples);
}

TimingStats timingStats;

// Send data to DS3231 over I2C. Set alarm1 and alarm2 registers to avoid
// changing the date and time. Total bytes sent: 11 bytes.
template <typename T_WIRE>
void sendData(T_WIRE& wireInterface) {
  wireInterface.beginTransmission(DS3231_I2C_ADDRESS); // one byte
  wireInterface.write(0x07); // start at position 7, one byte
  // Send 9 bytes to the alarm registers to preserve the
  // date and time stored on the DS3231.
  wireInterface.write(0x01); // alarm1 seconds
  wireInterface.write(0x02); // alarm1 minutes
  wireInterface.write(0x03); // alarm1 hours
  wireInterface.write(0x04); // alarm1 day
  wireInterface.write(0x05); // alarm1 date
  wireInterface.write(0x01); // alarm2 minutes
  wireInterface.write(0x02); // alarm2 hours
  wireInterface.write(0x03); // alarm2 day
  wireInterface.write(0x04); // alarm2 date
  wireInterface.endTransmission();
  // Total bytes: 11
}

template <typename T_WIRE>
void runBenchmark(
    const __FlashStringHelper* name, T_WIRE& wireInterface) {
  timingStats.reset();
  const uint16_t numSamples = 20;
  for (uint16_t i = 0; i < numSamples; ++i) {
    uint16_t startMicros = micros();
    sendData(wireInterface);
    uint16_t endMicros = micros();
    timingStats.update(endMicros - startMicros);
    yield();
  }

  printStats(name, timingStats, numSamples);
}

void runTwoWire100() {
  using WireInterface = TwoWireInterface<TwoWire>;
  WireInterface wireInterface(Wire);

  Wire.begin();
  Wire.setClock(100000L);
  wireInterface.begin();
  runBenchmark(F("TwoWireInterface<TwoWire>,100kHz"), wireInterface);
  wireInterface.end();
#if defined(ARDUINO_ARCH_AVR) \
    || defined(ARDUINO_ARCH_SAMD) \
    || defined(ARDUINO_ARCH_STM32) \
    || defined(TEENSYDUINO)
  Wire.end();
#endif
}

void runTwoWire400() {
  using WireInterface = TwoWireInterface<TwoWire>;
  WireInterface wireInterface(Wire);

  Wire.begin();
  Wire.setClock(400000L);
  wireInterface.begin();
  runBenchmark(F("TwoWireInterface<TwoWire>,400kHz"), wireInterface);
  wireInterface.end();
#if defined(ARDUINO_ARCH_AVR) \
    || defined(ARDUINO_ARCH_SAMD) \
    || defined(ARDUINO_ARCH_STM32) \
    || defined(TEENSYDUINO)
  Wire.end();
#endif
}

void runSimpleWire() {
  using WireInterface = SimpleWireInterface;
  WireInterface wireInterface(
      SDA_PIN, SCL_PIN, DELAY_MICROS);

  wireInterface.begin();
  runBenchmark(F("SimpleWireInterface,1us"), wireInterface);
  wireInterface.end();
}

#if defined(ARDUINO_ARCH_AVR) || defined(EPOXY_DUINO)
void runSimpleWireFast() {
  using WireInterface = SimpleWireFastInterface<SDA_PIN, SCL_PIN, DELAY_MICROS>;
  WireInterface wireInterface;

  wireInterface.begin();
  runBenchmark(F("SimpleWireFastInterface,1us"), wireInterface);
  wireInterface.end();
}
#endif

//-----------------------------------------------------------------------------
// runBenchmarks()
//-----------------------------------------------------------------------------

void runBenchmarks() {
  runTwoWire100();
  runTwoWire400();
  runSimpleWire();
#if defined(ARDUINO_ARCH_AVR) || defined(EPOXY_DUINO)
  runSimpleWireFast();
#endif
}

//-----------------------------------------------------------------------------
// sizeof()
//-----------------------------------------------------------------------------

void printSizeOf() {
  SERIAL_PORT_MONITOR.print(F("sizeof(TwoWireInterface): "));
  SERIAL_PORT_MONITOR.println(sizeof(TwoWireInterface<TwoWire>));

  SERIAL_PORT_MONITOR.print(F("sizeof(SimpleWireInterface): "));
  SERIAL_PORT_MONITOR.println(sizeof(SimpleWireInterface));

#if defined(ARDUINO_ARCH_AVR) || defined(EPOXY_DUINO)
  SERIAL_PORT_MONITOR.print(F("sizeof(SimpleWireFastInterface<2, 3, 10>): "));
  SERIAL_PORT_MONITOR.println(sizeof(SimpleWireFastInterface<2, 3, 10>));
#endif
}

//-----------------------------------------------------------------------------

void setup() {
#if ! defined(EPOXY_DUINO)
  delay(1000); // Wait for stability on some boards, otherwise garage on Serial
#endif

  SERIAL_PORT_MONITOR.begin(115200);
  while (!SERIAL_PORT_MONITOR); // Wait for Leonardo/Micro

  SERIAL_PORT_MONITOR.println(F("SIZEOF"));
  printSizeOf();

  SERIAL_PORT_MONITOR.println(F("BENCHMARKS"));
  runBenchmarks();

  SERIAL_PORT_MONITOR.println(F("END"));

#if defined(EPOXY_DUINO)
  exit(0);
#endif
}

void loop() {}
