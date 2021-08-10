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

#include <Arduino.h>
#include <Wire.h> // TwoWire
#include <AceCommon.h> // TimingStats
#include <AceWire.h>

// These work only for AVR.
#if defined(ARDUINO_ARCH_AVR)
  // https://github.com/Testato/SoftwareWire
  #include <SoftwareWire.h>
#endif

// These work for all architectures, but I don't want the extra maintenance
// under EpoxyDuino.
#if ! defined(EPOXY_DUINO)
  // https://github.com/RaemondBW/SWire
  #include <SWire.h>

  // https://github.com/felias-fogg/SlowSoftWire
  #include <SlowSoftWire.h>

  // https://github.com/Seeed-Studio/Arduino_Software_I2C
  #include <SoftwareI2C.h>
#endif

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

// Send data to DS3231 over I2C, checking for some errors along the way. Total
// bytes sent: 9 bytes.
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
  res = wireInterface.write(0x02); // alarm1 minutes
  res = wireInterface.write(0x03); // alarm1 hours
  res = wireInterface.write(0x04); // alarm1 day/date
  res = wireInterface.write(0x05); // alarm2 minutes
  res = wireInterface.write(0x06); // alarm2 hours
  res = wireInterface.write(0x07); // alarm2 day/date

  // For buffered implementations, this causes the actual bit transfers. For
  // unbuffered implementations, this causes just the STOP condition to be sent.
  // Total transfer: 7 bytes
  res = wireInterface.endTransmission();
  if (res) SERIAL_PORT_MONITOR.println(F("Error: endTransmission()"));

  // Total bytes: 9
}

template <typename T_WIREI>
void runBenchmark(
    const __FlashStringHelper* name, T_WIREI& wireInterface) {
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

// Use built-in <Wire.h> at 100 kHz
void runTwoWire100() {
#if defined(ESP32)
  // Create our own instance of TwoWire instead of using the pre-defined Wire or
  // Wire1 instances on ESP32 because we need to release the I2C bus at the end
  // of this function, but the ESP32 version of TwoWire does not expose an end()
  // method. Instead, it uses the destructor to perform the cleanup, which we
  // cannot trigger when using the pre-defined Wire or Wire1 instances. By using
  // our own instance, the compiler will automatically trigger the destructor at
  // the end of the scope.
  TwoWire wire(0);
#else
  TwoWire& wire = Wire;
#endif

  using WireInterface = TwoWireInterface<TwoWire>;
  WireInterface wireInterface(wire);

  wire.begin();
  wire.setClock(100000L);
  wireInterface.begin();
  runBenchmark(F("TwoWireInterface<TwoWire>,100kHz"), wireInterface);
  wireInterface.end();

#if ! defined(ESP32) && ! defined(ESP8266)
  wire.end();
#endif
}

// Use built-in <Wire.h> at 400 kHz
void runTwoWire400() {
#if defined(ESP32)
  TwoWire wire(0);
#else
  TwoWire& wire = Wire;
#endif

  using WireInterface = TwoWireInterface<TwoWire>;
  WireInterface wireInterface(wire);

  wire.begin();
  wire.setClock(400000L);
  wireInterface.begin();
  runBenchmark(F("TwoWireInterface<TwoWire>,400kHz"), wireInterface);
  wireInterface.end();

#if ! defined(ESP32) && ! defined(ESP8266)
  wire.end();
#endif
}

// Use AceWire/SimpleWireInterface
void runSimpleWire() {
  using WireInterface = SimpleWireInterface;
  WireInterface wireInterface(SDA_PIN, SCL_PIN, DELAY_MICROS);

  wireInterface.begin();
  runBenchmark(F("SimpleWireInterface,1us"), wireInterface);
  wireInterface.end();
}

#if defined(ARDUINO_ARCH_AVR) || defined(EPOXY_DUINO)
// Use AceWire/SimpleWireFastInterface
void runSimpleWireFast() {
  using WireInterface = SimpleWireFastInterface<SDA_PIN, SCL_PIN, DELAY_MICROS>;
  WireInterface wireInterface;

  wireInterface.begin();
  runBenchmark(F("SimpleWireFastInterface,1us"), wireInterface);
  wireInterface.end();
}
#endif

#if defined(ARDUINO_ARCH_AVR)
// Use https://github.com/Testato/SoftwareWire at 100 kHz
void runSoftwareWire100() {
  SoftwareWire softwareWire(SDA_PIN, SCL_PIN);
  using WireInterface = TwoWireInterface<SoftwareWire>;
  WireInterface wireInterface(softwareWire);

  softwareWire.begin();
  softwareWire.setClock(100000L);
  wireInterface.begin();
  runBenchmark(F("TwoWireInterface<SoftwareWire>,100kHz"), wireInterface);
  wireInterface.end();
  softwareWire.end();
}

// Use https://github.com/Testato/SoftwareWire at 400 kHz
void runSoftwareWire400() {
  SoftwareWire softwareWire(SDA_PIN, SCL_PIN);
  using WireInterface = TwoWireInterface<SoftwareWire>;
  WireInterface wireInterface(softwareWire);

  softwareWire.begin();
  softwareWire.setClock(400000L);
  wireInterface.begin();
  runBenchmark(F("TwoWireInterface<SoftwareWire>,400kHz"), wireInterface);
  wireInterface.end();
  softwareWire.end();
}
#endif

#if ! defined(EPOXY_DUINO)

// Use https://github.com/RaemondBW/SWire
void runSWire() {
  SoftWire swire;
  using WireInterface = TwoWireInterface<SoftWire>;
  WireInterface wireInterface(swire);

  swire.begin(SDA_PIN, SCL_PIN);
  wireInterface.begin();
  runBenchmark(F("TwoWireInterface<SWire>"), wireInterface);
  wireInterface.end();
  // no swire.end() defined
}

// Use https://github.com/felias-fogg/SlowSoftWire
void runSlowSoftWire() {
  SlowSoftWire slowSoftWire(SDA_PIN, SCL_PIN);
  using WireInterface = TwoWireInterface<SlowSoftWire>;
  WireInterface wireInterface(slowSoftWire);

  slowSoftWire.begin();
  wireInterface.begin();
  runBenchmark(F("TwoWireInterface<SlowSoftWire>"), wireInterface);
  wireInterface.end();
  // no slowSoftWire.end() (but is declared in header file)
}

// Use https://github.com/Seeed-Studio/Arduino_Software_I2C
void runSeeedSoftwareI2C() {
  SoftwareI2C seeedWire;
  using WireInterface = TwoWireInterface<SoftwareI2C>;
  WireInterface wireInterface(seeedWire);

  seeedWire.begin(SDA_PIN, SCL_PIN);
  wireInterface.begin();
  runBenchmark(F("TwoWireInterface<SeeedSoftwareI2C>"), wireInterface);
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

#if defined(ARDUINO_ARCH_AVR)
  runSoftwareWire100();
  runSoftwareWire400();
#endif

#if ! defined(EPOXY_DUINO)
  runSWire();
  runSlowSoftWire();
  runSeeedSoftwareI2C();
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
