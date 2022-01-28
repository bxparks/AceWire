/*
 * A program which compiles various AceWire implementations to determine the
 * flash and static memory sizes from the output of the compiler. See the
 * generated README.md for details.
 */

#include <Arduino.h>

// DO NOT MODIFY THIS LINE. This will be overwritten by collect.sh on each
// iteration, incrementing from 0 to N. The Arduino IDE will compile the
// program, then the script will extract the flash and static memory usage
// numbers printed out by the Arduino compiler. The numbers will be printed on
// the STDOUT, which then can be saved to a file specific for a particular
// hardware platform, e.g. "nano.txt" or "esp8266.txt".
#define FEATURE 0

// List of features of AceWire that we want to gather memory usage numbers.
#define FEATURE_BASELINE 0

// SimpleWireInterface (all platforms)
#define FEATURE_SIMPLE_WIRE 1

// SimpleWireFastInterface (AVR only)
#define FEATURE_SIMPLE_WIRE_FAST 2

// TwoWireInterface from <Wire.h> (all platforms)
#define FEATURE_TWO_WIRE 3

// https://github.com/felias-fogg/SlowSoftWire (all platforms)
#define FEATURE_FELIAS_FOGG_WIRE 4

// https://github.com/stevemarple/SoftWire (all platforms)
#define FEATURE_MARPLE_WIRE 5

// https://github.com/RaemondBW/SWire (all platforms)
#define FEATURE_RAEMOND_WIRE 6

// https://github.com/Seeed-Studio/Arduino_Software_I2C (all platforms)
#define FEATURE_SEEED_WIRE 7

// https://github.com/Testato/SoftwareWire (AVR only)
#define FEATURE_TESTATO_WIRE 8

// https://github.com/thexeno/HardWire-Arduino-Library (AVR, but not ATTINY)
#define FEATURE_THEXENO_WIRE 9

// https://github.com/todbot/SoftI2CMaster (AVR, but not ATTINY)
#define FEATURE_TODBOT_WIRE 10

// A volatile integer to prevent the compiler from optimizing away the entire
// program.
volatile int disableCompilerOptimization = 0;

#if FEATURE > FEATURE_BASELINE
  #include <AceWire.h>
  #if defined(ARDUINO_ARCH_AVR) || defined(EPOXY_DUINO)
    #include <digitalWriteFast.h>
  #endif
  using namespace ace_wire;

  const uint8_t SDA_PIN = 2;
  const uint8_t SCL_PIN = 3;
  const uint8_t DELAY_MICROS = 1;
  const uint8_t DS3231_I2C_ADDRESS = 0x68;

  #if FEATURE == FEATURE_SIMPLE_WIRE
    using WireInterface = SimpleWireInterface;
    WireInterface wireInterface(SDA_PIN, SCL_PIN, DELAY_MICROS);

  #elif FEATURE == FEATURE_SIMPLE_WIRE_FAST
    #if ! defined(ARDUINO_ARCH_AVR) && ! defined(EPOXY_DUINO)
      #error Unsupported FEATURE on this platform
    #endif

    #include <digitalWriteFast.h>
    #include <ace_wire/SimpleWireFastInterface.h>
    using WireInterface = SimpleWireFastInterface<
        SDA_PIN, SCL_PIN, DELAY_MICROS>;
    WireInterface wireInterface;

  #elif FEATURE == FEATURE_TWO_WIRE
    #include <Wire.h>
    using WireInterface = TwoWireInterface<TwoWire>;
    WireInterface wireInterface(Wire);

  #elif FEATURE == FEATURE_FELIAS_FOGG_WIRE
    #include <SlowSoftWire.h>
    SlowSoftWire slowSoftWire(SDA_PIN, SCL_PIN);
    using WireInterface = FeliasFoggWireInterface<SlowSoftWire>;
    WireInterface wireInterface(slowSoftWire);

  #elif FEATURE == FEATURE_MARPLE_WIRE
    #include <SoftWire.h>
    const uint8_t SOFT_WIRE_BUFFER_SIZE = 32;
    uint8_t rxBuffer[SOFT_WIRE_BUFFER_SIZE];
    uint8_t txBuffer[SOFT_WIRE_BUFFER_SIZE];
    SoftWire softWire(SDA_PIN, SCL_PIN);
    using WireInterface = MarpleWireInterface<SoftWire>;
    WireInterface wireInterface(softWire);

  #elif FEATURE == FEATURE_RAEMOND_WIRE
    #include <SWire.h>
    using WireInterface = RaemondWireInterface<SoftWire>;
    WireInterface wireInterface(SWire);

  #elif FEATURE == FEATURE_SEEED_WIRE
    #include <SoftwareI2C.h>
    SoftwareI2C seeedWire;
    using WireInterface = SeeedWireInterface<SoftwareI2C>;
    WireInterface wireInterface(seeedWire);

  #elif FEATURE == FEATURE_TESTATO_WIRE
    // AVR only
    #if defined(ARDUINO_ARCH_AVR)
      #include <SoftwareWire.h>
      SoftwareWire softwareWire(SDA_PIN, SCL_PIN);
      using WireInterface = TestatoWireInterface<SoftwareWire>;
      WireInterface wireInterface(softwareWire);
    #else
      #error Unsupported FEATURE on this platform
    #endif

  #elif FEATURE == FEATURE_THEXENO_WIRE
    // AVR only, but not ATTINYX5
    #if defined(ARDUINO_ARCH_AVR) && ! defined(ARDUINO_AVR_ATTINYX5)
      #include <HardWire.h>
      using WireInterface = ThexenoWireInterface<TwoWire>;
      WireInterface wireInterface(Wire);
    #else
      #error Unsupported FEATURE on this platform
    #endif

  #elif FEATURE == FEATURE_TODBOT_WIRE
    // AVR only, but not ATTINYX5
    #if defined(ARDUINO_ARCH_AVR) && ! defined(ARDUINO_AVR_ATTINYX5)
      #include <SoftI2CMaster.h>
      SoftI2CMaster todbotWire(SCL_PIN, SDA_PIN);
      using WireInterface = TodbotWireInterface<SoftI2CMaster>;
      WireInterface wireInterface(todbotWire);
    #else
      #error Unsupported FEATURE on this platform
    #endif


  #else
    #error Unknown FEATURE

  #endif
#endif

// TeensyDuino seems to pull in malloc() and free() when a class with virtual
// functions is used polymorphically. This causes the memory consumption of
// FEATURE_BASELINE (which normally has no classes defined, so does not include
// malloc() and free()) to be artificially small which throws off the memory
// consumption calculations for all subsequent features. Let's define a
// throw-away class and call its method for all FEATURES, including BASELINE.
#if defined(TEENSYDUINO)
  class FooClass {
    public:
      virtual void doit() {
        disableCompilerOptimization = 0;
      }
  };

  FooClass* foo;
#endif

void setup() {
#if defined(TEENSYDUINO)
  // Force Teensy to bring in malloc(), free() and other things for virtual
  // dispatch.
  foo = new FooClass();
#endif

disableCompilerOptimization = 3;

#if FEATURE == FEATURE_BASELINE
  // Do nothing

#elif FEATURE == FEATURE_SIMPLE_WIRE
  wireInterface.begin();

#elif FEATURE == FEATURE_SIMPLE_WIRE_FAST
  wireInterface.begin();

#elif FEATURE == FEATURE_TWO_WIRE
  Wire.begin();
  wireInterface.begin();

#elif FEATURE == FEATURE_FELIAS_FOGG_WIRE
  slowSoftWire.begin();
  wireInterface.begin();

#elif FEATURE == FEATURE_MARPLE_WIRE
  softWire.setRxBuffer(rxBuffer, SOFT_WIRE_BUFFER_SIZE);
  softWire.setTxBuffer(txBuffer, SOFT_WIRE_BUFFER_SIZE);
  softWire.begin();
  wireInterface.begin();

#elif FEATURE == FEATURE_RAEMOND_WIRE
  SWire.begin(SDA_PIN, SCL_PIN);
  wireInterface.begin();

#elif FEATURE == FEATURE_SEEED_WIRE
  seeedWire.begin(SDA_PIN, SCL_PIN);
  wireInterface.begin();

#elif FEATURE == FEATURE_TESTATO_WIRE
  softwareWire.begin();
  wireInterface.begin();

#elif FEATURE == FEATURE_THEXENO_WIRE
  Wire.begin();
  wireInterface.begin();

#elif FEATURE == FEATURE_TODBOT_WIRE
  Wire.begin();
  wireInterface.begin();

#else
  #error Unknown FEATURE

#endif
}

void loop() {
#if defined(TEENSYDUINO)
  foo->doit();
#endif

#if FEATURE == FEATURE_BASELINE
  // do nothing

#elif FEATURE == FEATURE_SIMPLE_WIRE \
    || FEATURE == FEATURE_SIMPLE_WIRE_FAST \
    || FEATURE == FEATURE_TWO_WIRE \
    || FEATURE == FEATURE_RAEMOND_WIRE \
    || FEATURE == FEATURE_FELIAS_FOGG_WIRE \
    || FEATURE == FEATURE_SEEED_WIRE \
    || FEATURE == FEATURE_MARPLE_WIRE \
    || FEATURE == FEATURE_TESTATO_WIRE \
    || FEATURE == FEATURE_THEXENO_WIRE \
    || FEATURE == FEATURE_TODBOT_WIRE
  wireInterface.beginTransmission(DS3231_I2C_ADDRESS);
  wireInterface.write(0x00);
  wireInterface.endTransmission();

  wireInterface.requestFrom(DS3231_I2C_ADDRESS, 1);
  wireInterface.read();

#else
  #error Unknown FEATURE

#endif
}
