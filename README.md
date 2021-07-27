# AceWire

Wrapper classes that provide a simple, unified interface for different I2C
implementations on Arduino platforms. The code was initially part of the
[AceSegment](https://github.com/bxparks/AceSegment) library, but was extracted
into a separate library. It has 3 primary purposes:

1. Allow client applications to easily select alternate I2C libraries instead of
   being locked into the pre-installed `<Wire.h>` library. Alternative libraries
   may be desirable for various reasons:
    * To use different GPIO pins, instead of the hardware `SDA` and `SCL`.
    * To reduce flash and static memory consumption compared to `<Wire.h>`.
    * To use packet sizes larger than the default 32-bytes provided by AVR
      `<Wire.h>`.
2. Prevent unnecessary flash memory consumption by preventing `AceSegment`
   from being forced to include `<Wire.h>` when it is not necessary.
    * Simply including the `<Wire.h>` header file causes memory consumption of
      the application to increase by 1140 bytes of flash and 113 bytes of static
      memory. This happens even if the application never uses anything defined
      by the `<Wire.h>` library.
    * The `AceSegment` library provides classes which use the SPI interface, and
      other classes which use the I2C interface. If it referenced the `<SPI.h>`
      and `<Wire.h>` header files directly, then client applications which used
      only the SPI interface would waste 1140 bytes of flash and 113 bytes of
      ram.
    * By going through an intermediary library like `AceWire` (and `AceSPI` and
      `AceTMI`), client applications do not suffer from unnecessary memory
      bloat.
3. Allow all libraries to share a common framework for selection a common I2C
   implementation within a given application.
    * Otherwise, some parts of the app would use `<Wire.h>`, and other parts of
      the app would use a different I2C implementation.

The `TwoWire` class in default `<Wire.h>` library cannot be used
polymorphically. In other words, subclasses cannot be used through a pointer (or
reference) to the base `TwoWire` class. To get around this problem, this library
uses C++ templates to provide compile-time polymorphism instead of runtime
polymorphism. This also means that the calling application code pays only a
minimal or zero-cost runtime overhead for the abstraction, by avoiding the
`virtual` dispatch.

The library provides an I2C wrapper class (`TwoWireInterface`) around other
third party I2C implementations, as well as 2 of its own software I2C
implementations (`SimpleWireInterface`, `SimpleWireFastInterface`):

* `TwoWireInterface.h`
    * Thin wrapper around an actual I2C implementation library.
    * Compatible with the `TwoWire` class in the pre-installed `<Wire.h>`
      library.
    * Compatible with other hardware and software implementations are supported
      as long as they implement a handful of methods that are syntactically
      compatible with the `TwoWire` class. At least 5 different third party
      libraries have been verified to work with `TwoWireInterface`:
        * [Using Third Party I2C Libraries](#UsingThirdPartyI2CLibraries)
        * [Third Party Compatibility](#ThirdPartyCompatibility)
* `SimpleWireInterface.h`
    * AceWire's own software bitbanging implementation that supports writing
      and reading from simple I2C devices, such as an HT16K33 LED controller
      chip and a DS3231 RTC chip.
* `SimpleWireFastInterface.h`
    * Same as `SimpleWireInterface.h` using one of the `<digitalWriteFast.h>`
      libraries that's available on AVR processors.
    * Can be up to 4X faster than `SimpleWireInterface` on AVR processors.
    * Can consume about 10X less flash memory than `<Wire.h>`, about 250 bytes
      of flash compared to 2500 bytes.

The library currently supports only a limited set of I2C functionality:

* master mode only, no slave
* no multi-master negotiation
* no clock stretching
* able to use any GPIO pin
* tested AVR, SAMD21, STM32, ESP8266, ESP32, and Teensy 3.2
* repeated start seems to work
* only 7-bit addresses are supported, 10-bit addresses are not supported

**Version**: 0.2+ (2021-07-19)

**Changelog**: [CHANGELOG.md](CHANGELOG.md)

**See Also**:
* https://github.com/bxparks/AceSPI
* https://github.com/bxparks/AceTMI

## Table of Contents

* [Installation](#Installation)
    * [Source Code](#SourceCode)
    * [Dependencies](#Dependencies)
* [Documentation](#Documentation)
* [Usage](#Usage)
    * [Include Header and Namespace](#HeaderAndNamespace)
    * [Unified Interface](#UnifiedInterface)
    * [TwoWireInterface](#TwoWireInterface)
    * [SimpleWireInterface](#SimpleWireInterface)
    * [SimpleWireFastInterface](#SimpleWireFastInterface)
    * [Storing Interface Objects](#StoringInterfaceObjects)
    * [Using Third Party I2C Libraries](#UsingThirdPartyI2CLibraries)
    * [ThirdPartyCompatibility](#ThirdPartyCompatibility)
    * [Writing to I2C](#WritingToI2C)
    * [Reading from I2C](#ReadingFromI2C)
    * [Error Handling](#ErrorHandling)
* [Resource Consumption](#ResourceConsumption)
    * [Flash And Static Memory](#FlashAndStaticMemory)
    * [CPU Cycles](#CpuCycles)
* [System Requirements](#SystemRequirements)
    * [Hardware](#Hardware)
    * [Tool Chain](#ToolChain)
    * [Operating System](#OperatingSystem)
* [License](#License)
* [Feedback and Support](#FeedbackAndSupport)
* [Authors](#Authors)

<a name="Installation"></a>
## Installation

The latest stable release will eventually be available in the Arduino IDE
Library Manager. Search for "AceWire". Click install. (It is not there
yet.)

The development version can be installed by cloning the
[GitHub repository](https://github.com/bxparks/AceWire), checking out the
`develop` branch, then manually copying over the contents to the `./libraries`
directory used by the Arduino IDE. (The result is a directory named
`./libraries/AceWire`.)

The `master` branch contains the stable release.

<a name="SourceCode"></a>
### Source Code

The source files are organized as follows:
* `src/AceWire.h` - main header file
* `src/ace_wire/` - implementation files
* `docs/` - contains the doxygen docs and additional manual docs

<a name="Dependencies"></a>
### Dependencies

The main `AceWire.h` does not depend any external libraries.

The `SimpleWireFastInterface.h` implementation depends on one of the
digitalWriteFast libraries, for example:

* https://github.com/watterott/Arduino-Libs/tree/master/digitalWriteFast
* https://github.com/NicksonYap/digitalWriteFast

<a name="Documentation"></a>
## Documentation

* this `README.md` file.
* [Doxygen docs](https://bxparks.github.io/AceWire/html)
    * On Github pages.
* Examples:
    * https://github.com/bxparks/AceSegment/tree/develop/examples/Ht16k33Demo

<a name="Usage"></a>
## Usage

<a name="HeaderAndNamespace"></a>
### Include Header and Namespace

In many cases, only a single header file `AceWire.h` is required to use this
library. To prevent name clashes with other libraries that the calling code may
use, all classes are defined in the `ace_wire` namespace.

To use the `TwoWireInterface` class, which is a wrapper around the pre-defined
`Wire` object from the pre-included `<Wire.h>` library, use:

```C++
#include <Wire.h>
#include <AceWire.h>
using ace_wire::TwoWireInterface;
```

To use `SimpleWireInterface`, AceWire's own software implementation of I2C, use
the following.

```C++
#include <AceWire.h>
using ace_wire::SimpleWireInterface;
```

You should *not* include `<Wire.h>`, because it is not needed by
`SimpleWireInterface`, and because `<Wire.h>` pulls in the `Wire` object which
consumes about 1000 bytes of flash on AVR processors even when it is never
used.

To use `SimpleWireFastInterface`, use:

```C++
#include <AceWire.h>
#if defined(ARDUINO_ARCH_AVR)
  #include <digitalWriteFast.h>
  #include <ace_wire/SimpleWireFastInterface.h>
  using ace_wire::SimpleWireFastInterface;
#endif
```

<a name="UnifiedInterface"></a>
### Unified Interface

The classes in this library provide the following unified interface for handling
I2C communication. Downstream classes can code against this unified interface
using C++ templates so that different implementations can be selected at
compile-time.

Some implementations use a transmit and receive buffer (e.g. `<Wire.h>`). Other
implementations do not use any buffers. Some split the difference, using no
buffer for transmit, but a buffer for receive. The unified interface attempts to
find a common API among these variations.

```C++
class XxxInterface {
  public:
    void begin();
    void end();

    uint8_t beginTransmission(uint8_t addr);
    uint8_t write(uint8_t data);
    uint8_t endTransmission(bool sendStop);
    uint8_t endTransmission();

    uint8_t requestFrom(uint8_t addr, uint8_t quantity, bool sendStop);
    uint8_t requestFrom(uint8_t addr, uint8_t quantity);
    uint8_t read();
    void endRequest();
};
```

The `beginTransmission()` method returns a 0 upon success. For implementations
using a transmit buffer, this will always return a success because nothing is
actually sent over the wire until the `endTransmission()`. For implementations
which do not use a buffer, this will send the `addr` byte on the I2C bus. If a
device responds with an ACK, then the method will return 0, otherwise it will
return a 1 to indicate failure.

The `write()` method returns 1 upon success, 0 upon failure. This allows
compatibility with the native `<Wire.h>` library which returns a `size_t`
representing the number of bytes actually written. In this API, the return type
is a `uint8_t` which is sufficient because only a single byte can be transferred
at a time. The `write()` method in the native `<Wire.h>` library always returns
1 because all simply writes the data byte into a buffer. But in non-buffered
implementations, the data is actually sent over the wire and this method returns
the ACK/NACK response of the slave device.

The `endTransmission()` method returns 0 upon success. The native `<Wire.h>
library and a few other libraries return other non-zero error codes are returned
depending on the error condition:

* 0: success
* 1: length too long for buffer
* 2: address send, NACK received
* 3: data send, NACK received
* 4: other twi error (lost bus arbitration, bus error, ..)

On non-buffered implementations, the `endTransmission()` method always returns a
0 because all of the data transmission already happened in the `write()` method
and the `endTransmission()` method simply sends a STOP condition if requested.

The `requestFrom()` method returns `quantity` if successful, or 0 if an error
occurs. In buffered implementations, all of the data transfer happens in this
method so a return value of `quantity` means the transfer was successful. But in
unbuffered implementations, this method simply sends the `addr` byte, so a 0
means that the slave device responded with a NACK.

The `read()` method returns the data byte from the slave. There is no ability to
detect an error condition from the slave in this method, mostly because it is
the master that sends the ACK or NACK bit to the slave. In both buffered and
unbuffered implementations, the `read()` should NOT be called if the
`requestFrom()` method returns failure (i.e. 0).

The `endRequest()` method is an extension added to the native `<Wire.h>` API
so that client applications can be mostly agnostic about the actual underlying
I2C implementation. For the `TwoWireInterface` wrapper, this method is an empty
method that does nothing. For the `SimpleWireInterface` and
`SimpleWireFastInterface`, this method sends a STOP condition to the slave
device.

Notice that the classes in this library do *not* inherit from a common interface
with virtual functions. This saves several hundred bytes of flash memory on
8-bit AVR processors by avoiding the dynamic dispatch. Often the
compiler is able to optimize away the overhead of calling the methods in this
library so that the function call is made directly to the underlying
implementation. The reduction of flash memory consumption is especially large
for classes that use the digitalWriteFast libraries which use compile-time
constants for pin numbers. The disadvantage is that this library is harder to
use because these classes require the downstream classes to be implemented using
C++ templates.

Also note that unlike many I2C libraries, including `<Wire.h>`, AceWire does
**not** provide an `available()` method. This is because the functionality of
that method cannot be implemented within the I2C specification. When the master
is reading from the slave, it is the master that sends the ACK/NACK bit to the
slave. There is no mechanism for the slave to tell the master when no more
bytes are available. If the slave happens to stop sending early, then the
SDA line will passively pull to HIGH, and the master will read `0xFF`.

In most I2C implementations, `available()` simply returns the number of bytes
requested (`quantity`) minus the number of times `read()` was called. There is
at least one implementation which instead returns the number of times that
`read()` was called. I recommend users to ignore the `available()` method and
always read `quantity` number of bytes. For additional information, see
[ArduinoCore-avr#384](https://github.com/arduino/ArduinoCore-avr/issues/384) and
[ArduinoCore-avr#171](https://github.com/arduino/ArduinoCore-avr/issues/171).

<a name="TwoWireInterface"></a>
### TwoWireInterface

The `TwoWireInterface` is a thin wrapper around the `TwoWire` class provided by
the pre-installed `<Wire.h>` library. It looks like this:

```C++
template <typename T_WIRE>
class TwoWireInterface {
  public:
    explicit TwoWireInterface(T_WIRE& wire) : mWire(wire) {}

    void begin() {...}
    void end() {...}

    uint8_t beginTransmission(uint8_t addr) {...}
    uint8_t write(uint8_t data) {...}
    uint8_t endTransmission(bool sendStop = true) {...}

    uint8_t requestFrom(uint8_t addr, uint8_t quantity, bool sendStop) {...}
    uint8_t requestFrom(uint8_t addr, uint8_t quantity) {...}
    uint8_t read() {...}
    void endRequest() {...}
};
```

It is flexible enough to become a wrapper around any I2C implementation class
(hardware or software) that implements an API similar enough to the `TwoWire`
class See [Using Third Party I2C Libraries](#UsingThirdPartyI2CLibraries) below.

It is configured and used by the calling code `MyClass` like this:

```C++
#include <Arduino.h>
#include <Wire.h>
#include <AceWire.h>
using ace_wire::TwoWireInterface;

template <typename T_WIRE>
class MyClass {
  private:
    static const uint8_t ADDRESS = ...;
    static const uint8_t NUM_BYTES = ...;
    static const bool SEND_STOP = true;

  public:
    explicit MyClass(T_WIRE& wireInterface)
        : mWireInterface(wireInterface)
    {...}

    void writeToDevice() {
      mWireInterface.beginTransmission(ADDRESS);
      mWireInterface.write(data1);
      mWireInterface.write(data2);
      ...
      mWireInterface.endTransmission();
    }

    void readFromDevice() {
      mWireInterface.requestFrom(ADDRESS, NUM_BYTES, SEND_STOP);
      uint8_t data1 = mWireInterface.read();
      uint8_t data2 = mWireInterface.read();
      ...
      mWireInterface.endRequest(); // this is important!
    }

  private:
    T_WIRE mWireInterface; // copied by value
};

using WireInterface = TwoWireInterface<TwoWire>;
WireInterface wireInterface(Wire);
MyClass<WireInterface> myClass(wireInterface);

void setup() {
  Wire.begin();
  wireInterface.begin();
  ...
}
```

The `using` statement is the C++11 version of a `typedef` that defines
`WireInterface`. It is not strictly necessary here, but it allows the same
code structure to be used for the more complicated examples below.

The `T_WIRE` template parameter contains a `T_` prefix to avoid name collisions
with too many `#define` macros defined in the global namespace on Arduino
platforms.

See the [Writing to I2C](#WritingToI2C) and [Reading from I2C](#ReadingFromI2C)
sections below for documentation about the `beginTransmission()`,
`endTransmission()`, `requestFrom()`, and `endRequest()` methods.

<a name="SimpleWireInterface"></a>
### SimpleWireInterface

The `SimpleWireInterface` is a software implementation of I2C that has just
enough functionality to communicate with some simple I2C devices, such as an
HT16K33 LED controller chip, or a DS3231 RTC chip. It currently supports just a
master mode, with no clock stretching. It looks like this:

```C++
class SimpleWireInterface {
  public:
    explicit SimpleWireInterface(
        uint8_t dataPin, uint8_t clockPin, uint8_t delayMicros
    ) :
        mDataPin(dataPin),
        mClockPin(clockPin),
        mDelayMicros(delayMicros)
    {}

    void begin() {...}
    void end() {...}

    uint8_t beginTransmission(uint8_t addr) {...}
    uint8_t write(uint8_t data) {...}
    uint8_t endTransmission(bool sendStop = true) {...}

    uint8_t requestFrom(uint8_t addr, uint8_t quantity, bool sendStop = true) {
      ...
    }
    uint8_t read() {...}
    void endRequest() {...}
};
```
It is configured and used by the calling code `MyClass` like this:

```C++
#include <Arduino.h>
#include <AceWire.h>
using ace_wire::SimpleWireInterface;

template <typename T_WIRE>
class MyClass {
  // Exactly the same as above.
};

const uint8_t SCL_PIN = SCL;
const uint8_t SDA_PIN = SDA;
const uint8_t DELAY_MICROS = 4;

using WireInterface = SimpleWireInterface;
WireInterface wireInterface(SDA_PIN, SCL_PIN, DELAY_MICROS);
MyClass<WireInterface> myClass(wireInterface);

void setup() {
  wireInterface.begin();
  ...
}
```

The `DELAY_MICROS` parameter is the number of microseconds to wait between
transitions of the SCL and SDA signals. The smallest value of this parameter
depends on the capacitance and resistance of the SCL and SDA lines, and the
capabilities of the target device, with smaller values requiring lower
capacitance and lower resistance. The largest value of this parameter depends
mostly on the capabilities of the target device.

The actual delay between the transitions of the SCL and SDA signal may be
significantly different from the `DELAY_MICROS` parameter for several reasons:

* The accuracy of the `delayMicroseconds()` function on AVR processors is
  signficantly degraded for values less than about 10 microseconds.
* The speed of the `digitalWrite()` function is known to be particularly slow
  on AVR processors, and it may take more time than the value of `DELAY_MICROS`.

Trial and error may be required to determine an appropriate value of
`DELAY_MICROS`.

**Important**: The `SimpleWireInterface` class does not need the `<Wire.h>`
library. You should *not* add an `#include <Wire.h>` statement in your program
if nothing else in your program needs it. Adding that single include line
increases the flash memory consumption on AVR processors by about 1140 bytes and
increases static ram consumption by 113 bytes, even if the `Wire` object is
never used.

<a name="SimpleWireFastInterface"></a>
### SimpleWireFastInterface

The `SimpleWireFastInterface` is the same as `SimpleWireInterface` but using the
`digitalWriteFast()` function. It looks like this:

```C++
template <
    uint8_t T_DATA_PIN,
    uint8_t T_CLOCK_PIN,
    uint8_t T_DELAY_MICROS
>
class SimpleWireFastInterface {
  public:
    explicit SimpleWireFastInterface() = default;

    void begin() {...}
    void end() {...}

    uint8_t beginTransmission(uint8_t addr) {...}
    uint8_t write(uint8_t data) {...}
    uint8_t endTransmission(bool sendStop = true) {...}

    uint8_t requestFrom(uint8_t addr, uint8_t quantity, bool sendStop = true) {
      ...
    }
    uint8_t read() {...}
    void endRequest() {...}
};
```

It is configured and used by the calling code `MyClass` like this:

```C++
#include <Arduino.h>
#include <AceWire.h>
#if defined(ARDUINO_ARCH_AVR)
  #include <digitalWriteFast.h>
  #include <ace_wire/SimpleWireFastInterface.h>
  using ace_wire::SimpleWireFastInterface;
#endif

template <typename T_WIRE>
class MyClass {
  // Exactly the same as above.
};

const uint8_t SCL_PIN = SCL;
const uint8_t SDA_PIN = SDA;
const uint8_t DELAY_MICROS = 4;

using WireInterface = SimpleWireFastInterface<SDA_PIN, SCL_PIN, DELAY_MICROS>;
WireInterface wireInterface;
MyClass<WireInterface> myClass(wireInterface);

void setup() {
  wireInterface.begin();
  ...
}
```

**Important**: The `SimpleWireFastInterface` class does not need the `<Wire.h>`
library. You should *not* add an `#include <Wire.h>` statement in your program
if nothing else in your program needs it. Adding that single include line
increases the flash memory consumption on AVR processors by about 1140 bytes and
increases static ram consumption by 113 bytes, even if the `Wire` object is
never used.

<a name="StoringInterfaceObjects"></a>
### Storing Interface Objects

In the above examples, the `MyClass` object holds the `T_WIRE` interface object
**by value**. In other words, the interface object is copied into the `MyClass`
object. This is efficient because interface objects are very small in size, and
copying them by-value avoids an extra level of indirection when they are used
inside the `MyClass` object. The compiler will generate code that is equivalent
to calling the underlying `Wire` methods through the `TwoWire` pointer.

The alternative is to save the `T_WIRE` object **by reference** like this:

```C++
template <typename T_WIRE>
class MyClass {
  public:
    explicit MyClass(T_WIRE& wireInterface)
        : mWireInterface(wireInterface)
    {...}

    [...]

  private:
    T_WIRE& mWireInterface; // copied by reference
};
```

The internal size of the `TwoWireInterface` object is just a single reference to
the `T_WIRE` object, so there is no difference in the static memory size.
However, storing the `mWireInterface` as a reference causes an unnecessary extra
layer of indirection every time the `mWireInterface` object is called. In almost
every case, I recommend storing the `XxxInterface` object by value into the
`MyClass` object.

<a name="UsingThirdPartyI2CLibraries"></a>
### Using Third Party I2C Libraries

The usefulness of the `TwoWireInterface` class is that it is not restricted to
just the `TwoWire` class from the `<Wire.h>` library. It can be actually be used
with any third party I2C library that implements an API similar enough to the
`TwoWire` class from the `<Wire.h>` library. This is useful because the
`TwoWire` class does not support polymorphism and cannot be subclassed (see
the discussions in
[Testato/SoftwareWire#32](https://github.com/Testato/SoftwareWire/pull/32) and
[Testato/SoftwareWire#28](https://github.com/Testato/SoftwareWire/issues/28).)

Here is an example of using `TwoWireInterface` with
[SoftwareWire](https://github.com/Testato/SoftwareWire). The `SoftwareWire`
class implements an API which is almost identical to `TwoWire`. Since
`TwoWireInterface` is a template class, it does not use inheritance, so it can
be used with `SoftwareWire` as shown below:

```C++
#include <Arduino.h>
#include <SoftwareWire.h>
#include <AceWire.h>
using ace_wire::TwoWireInterface;

template <typename T_WIRE>
class MyClass {
  // Exactly same as above.
};

const uint8_t SCL_PIN = SCL;
const uint8_t SDA_PIN = SDA;

SoftwareWire softwareWire(SDA_PIN, SCL_PIN);
using WireInterface = TwoWireInterface<SoftwareWire>;
WireInterface wireInterface(softwareWire);
MyClass<WireInterface> myClass(wireInterface);

void setup() {
  softwareWire.begin();
  wireInterface.begin();
  ...
}
```

The critical point is that `MyClass<T_WIRE>` remains unchanged from the previous
example, and everything should just work.

<a name="ThirdPartyCompatibility"></a>
### Third Party Compatibility

The following third party I2C libraries (both hardware and software
implementations) have been verified to work with the `TwoWireInterface` wrapper
class:

* https://github.com/Testato/SoftwareWire
* https://github.com/RaemondBW/SWire
* https://github.com/felias-fogg/SlowSoftWire
* https://github.com/thexeno/HardWire-Arduino-Library
* https://github.com/Seeed-Studio/Arduino_Software_I2C

I could not get the following to work:

* https://github.com/stevemarple/SoftWire
    * Nothing happens.
* https://github.com/felias-fogg/SoftI2CMaster, using its `SoftWire.h`
    * On a SparkFun Pro Micro, nothing happens.
    * On an Arduino Nano, the assembler fails with a fatal error:
      "Error: number must be positive and less than 8".

See https://github.com/bxparks/AceSegment/tree/develop/examples/Ht16k33Demo
for the configuration of each library.

For other third party I2C libraries which do not implement the API structure of
`TwoWire`, it is possible to create a customized version of `TwoWireInterface`.
Each third party library would need to be handled on a case-by-case basis. In
return for this additional work of going through the AceWire library, we gain
the ability to use different I2C libraries with only a minimal amount of
configuration changes, while paying only a small or even zero runtime overhead
by taking advantage of C++ templates.

<a name="WritingToI2C"></a>
### Writing to I2C

Once the `T_WIRE mWireInstnace` is created in the `MyClass<T_WIRE>` class,
writing to the I2C device is basically the same as using the `Wire` object
directly:

```C++
template <typename T_WIRE>
class MyClass {
  private:
    static const uint8_t ADDRESS = ...;
    ...

  public:
    void writeToDevice() {
      mWireInterface.beginTransmission(ADDRESS);
      mWireInterface.write(data1);
      mWireInterface.write(data2);
      ...
      mWireInterface.endTransmission();
    }

  private:
    T_WIRE mWireInterface; // copied by value
};
```

Some I2C implementations (e.g. `Wire`, `SoftwareWire`) follow the `TwoWire`
class from `<Wire.h>`, and use an internal buffer (often 32 bytes on 8-bit
platforms) to store the data. The `write(uint8_t)` method does not actually
write data to the bus, rather it writes into the buffer. The actual transmission
of the data does not occur until `endTransmission()` is called. Even if the
implementation uses hardware interrupts, the `endTransmission()` method will be
a blocking call that does not return until all the bytes in the write buffer are
sent.

Some I2C implementation do not use a write buffer (e.g. `SimpleWireInterface`,
`SimpleWireFastInterface`, and `SlowSoftWire` below.). Instead, the
`beginTransmission()`, `write()`, and `endTransmission()` methods directly write
the necessary data to the I2C bus.

For compatibility with both types of implementations, the code in `MyClass` that
uses AceWire classes should assume that the write buffer does *not* exist. The
calling code should assume that the `beginTransmission()`, `write()`, and
`endTransmission()` are all blocking calls which send out the data to the I2C
immediately. Therefore, the `write()` method should be called as quickly as
possible.

<a name="ReadingFromI2C"></a>
### Reading from I2C

Reading from the I2C bus is similar to writing. Once the `mWireInterface` object
is available in the `MyClass` object, we can read bytes from the I2C like this:

```C++
template <typename T_WIRE>
class MyClass {
  private:
    static const uint8_t ADDRESS = ...;
    static const uint8_t NUM_BYTES = ...;
    static const bool SEND_STOP = true;
    ...

  public:
    void readFromDevice() {
      mWireInterface.requestFrom(ADDRESS, NUM_BYTES, SEND_STOP);
      uint8_t data1 = mWireInterface.read();
      uint8_t data2 = mWireInterface.read();
      ...
      mWireInterface.endRequest();
    }

  private:
    T_WIRE mWireInterface; // copied by value
};
```

The `requestFrom()` and `read()` methods should look familiar to those who have
used the `TwoWire` class from `<Wire.h>`.

The `endRequest()` method is new. It is a no-op function for `TwoWireInterface`
because it assumes that the underlying implementations use a receive buffer, so
all the work was already done in the `requestFrom()` method. However, the
`SimpleWireInterface` and `SimpleWireFastInterface` implementations do not use a
buffer. They extract the data stream directly from the I2C bus using
`digitalRead()` or `digitalReadFast()` methods. So the `endRequest()` method
must be called by the `MyClass` calling class, so that the I2C "stop" condition
can be sent to the device according to the I2C specification.

To be compatible with both types of implementations (ones with a receive buffer
and ones without), the calling code must pair each `requestFrom()` call with a
matching call to `endRequest()` at the end of the transaction.

The `SEND_STOP` boolean flag indicates whether the I2C master will send a "stop"
condition after reading the required number of bytes. Usually it is a good idea
to send a "stop" condition, because not all slave devices nor all I2C libraries
support the "repeated start" feature of the I2C bus.

**Important**: For all implementations, the number of calls to the `read()`
method must be *exactly* equal to `NUM_BYTES`. Otherwise, unpredictable things
may happen, including a crash of the system.

<a name="ErrorHandling"></a>
### Error Handling

Performing proper error handling for the various I2C libraries is difficult.
Different I2C implementations handle errors in slightly different ways. For
many non-critical applications with simple hardware configurations, with only a
few I2C devices on the bus, it may be practical to just ignore all reported
errors from the underlying library.

Some error checking is possible by checking the return values of
`beginTransmission()`, `write()` and `endTransmission()`. You need to keep in
mind that the underlying I2C library may trigger the error conditions at
different points in the API flow, depending on whether the implementation uses
buffers or not, and the error codes returned may be slightly different.

The software I2C implementations provided in this library (`SimpleWireInterface`
and `SimpleWireFastInterface`) do not implement clock stretching, and they do
not check to see if another I2C master is controlling the bus. Therefore, they
cannot become wedged into an infinite loop so they do not provide a timeout
parameter.

The hardware `<Wire.h>` has the potential for becoming wedged. Recently, some
work was been done to allow the library to time out after a certain amount of
time. (I am not too familiar with those latest feature additions.)

Other third party libaries (accessed through the `TwoWireInterface` wrapper
class) may handle errors in different ways. Usually, the error handling behavior
of the code is not documented so you probably need to dive into the code.

<a name="ResourceConsumption"></a>
## Resource Consumption

<a name="FlashAndStaticMemory"></a>
### Flash And Static Memory

The Memory benchmark numbers can be seen in
[examples/MemoryBenchmark](examples/MemoryBenchmark). Here are 2 samples:

**Arduino Nano**

```
+------------------------------------------------------------------+
| functionality                       |  flash/  ram |       delta |
|-------------------------------------+--------------+-------------|
| baseline                            |    456/   11 |     0/    0 |
|-------------------------------------+--------------+-------------|
| TwoWireInterface<TwoWire>           |   2926/  229 |  2470/  218 |
| SimpleWireInterface                 |   1320/   16 |   864/    5 |
| SimpleWireFastInterface             |    698/   13 |   242/    2 |
|-------------------------------------+--------------+-------------|
| TwoWireInterface<SoftwareWire>      |   2454/   72 |  1998/   61 |
| TwoWireInterface<SWire>             |   1698/  157 |  1242/  146 |
| TwoWireInterface<SlowSoftWire>      |   2008/   83 |  1552/   72 |
| TwoWireInterface<SeeedSoftwareI2C>  |   1614/   21 |  1158/   10 |
+------------------------------------------------------------------+
```

**ESP8266**

```
+------------------------------------------------------------------+
| functionality                       |  flash/  ram |       delta |
|-------------------------------------+--------------+-------------|
| baseline                            | 256700/26784 |     0/    0 |
|-------------------------------------+--------------+-------------|
| TwoWireInterface<TwoWire>           | 261404/27268 |  4704/  484 |
| SimpleWireInterface                 | 257764/26804 |  1064/   20 |
|-------------------------------------+--------------+-------------|
| TwoWireInterface<SWire>             | 258408/26944 |  1708/  160 |
| TwoWireInterface<SlowSoftWire>      | 259972/26884 |  3272/  100 |
| TwoWireInterface<SeeedSoftwareI2C>  | 257956/26812 |  1256/   28 |
+------------------------------------------------------------------+
```

<a name="CpuCycles"></a>
### CPU Cycles

The CPU benchmark numbers can be seen in
[examples/AutoBenchmark](examples/AutoBenchmark). Here are 2 samples:

**Arduino Nano**

```
+-----------------------------------------+-------------------+----------+
| Functionality                           |   min/  avg/  max | eff kbps |
|-----------------------------------------+-------------------+----------|
| TwoWireInterface<TwoWire>,100kHz        |  1132/ 1137/ 1160 |     87.1 |
| TwoWireInterface<TwoWire>,400kHz        |   376/  380/  392 |    260.5 |
| SimpleWireInterface,1us                 |  2004/ 2021/ 2212 |     49.0 |
| SimpleWireFastInterface,1us             |   168/  181/  192 |    547.0 |
|-----------------------------------------+-------------------+----------|
| TwoWireInterface<SoftwareWire>,100kHz   |  1660/ 1670/ 1740 |     59.3 |
| TwoWireInterface<SoftwareWire>,400kHz   |  1196/ 1206/ 1328 |     82.1 |
| TwoWireInterface<SWire>                 |  3068/ 3085/ 3380 |     32.1 |
| TwoWireInterface<SlowSoftWire>          |  2260/ 2274/ 2500 |     43.5 |
| TwoWireInterface<SeeedSoftwareI2C>      |  1784/ 1804/ 1972 |     54.9 |
+-----------------------------------------+-------------------+----------+
```

**ESP8266**

```
+-----------------------------------------+-------------------+----------+
| Functionality                           |   min/  avg/  max | eff kbps |
|-----------------------------------------+-------------------+----------|
| TwoWireInterface<TwoWire>,100kHz        |  1031/ 1040/ 1205 |     95.2 |
| TwoWireInterface<TwoWire>,400kHz        |   270/  270/  274 |    366.7 |
| SimpleWireInterface,1us                 |  1037/ 1040/ 1096 |     95.2 |
|-----------------------------------------+-------------------+----------|
| TwoWireInterface<SWire>                 |  1130/ 1134/ 1206 |     87.3 |
| TwoWireInterface<SlowSoftWire>          |  1051/ 1056/ 1143 |     93.8 |
| TwoWireInterface<SeeedSoftwareI2C>      |   383/  386/  444 |    256.5 |
+-----------------------------------------+-------------------+----------+
```

<a name="SystemRequirements"></a>
## System Requirements

<a name="Hardware"></a>
### Hardware

This library has Tier 1 support on the following boards:

* Arduino Nano (16 MHz ATmega328P)
* SparkFun Pro Micro (16 MHz ATmega32U4)
* SAMD21 M0 Mini (48 MHz ARM Cortex-M0+)
* STM32 Blue Pill (STM32F103C8, 72 MHz ARM Cortex-M3)
* NodeMCU 1.0 (ESP-12E module, 80MHz ESP8266)
* WeMos D1 Mini (ESP-12E module, 80 MHz ESP8266)
* ESP32 dev board (ESP-WROOM-32 module, 240 MHz dual core Tensilica LX6)
* Teensy 3.2 (72 MHz ARM Cortex-M4)

Tier 2 support can be expected on the following boards, mostly because I don't
test these as often:

* ATtiny85 (8 MHz ATtiny85)
* Arduino Pro Mini (16 MHz ATmega328P)
* Teensy LC (48 MHz ARM Cortex-M0+)
* Mini Mega 2560 (Arduino Mega 2560 compatible, 16 MHz ATmega2560)

The following boards are **not** supported:

* Any platform using the ArduinoCore-API
  (https://github.com/arduino/ArduinoCore-api). For example:
    * Nano Every
    * MKRZero
    * Raspberry Pi Pico RP2040

<a name="ToolChain"></a>
### Tool Chain

* [Arduino IDE 1.8.13](https://www.arduino.cc/en/Main/Software)
* [Arduino CLI 0.14.0](https://arduino.github.io/arduino-cli)
* [SpenceKonde ATTinyCore 1.5.2](https://github.com/SpenceKonde/ATTinyCore)
* [Arduino AVR Boards 1.8.3](https://github.com/arduino/ArduinoCore-avr)
* [Arduino SAMD Boards 1.8.9](https://github.com/arduino/ArduinoCore-samd)
* [SparkFun AVR Boards 1.1.13](https://github.com/sparkfun/Arduino_Boards)
* [SparkFun SAMD Boards 1.8.3](https://github.com/sparkfun/Arduino_Boards)
* [STM32duino 2.0.0](https://github.com/stm32duino/Arduino_Core_STM32)
* [ESP8266 Arduino 2.7.4](https://github.com/esp8266/Arduino)
* [ESP32 Arduino 1.0.6](https://github.com/espressif/arduino-esp32)
* [Teensyduino 1.53](https://www.pjrc.com/teensy/td_download.html)

<a name="OperatingSystem"></a>
### Operating System

I use Ubuntu 20.04 for the vast majority of my development. I expect that the
library will work fine under MacOS and Windows, but I have not explicitly tested
them.

<a name="License"></a>
## License

[MIT License](https://opensource.org/licenses/MIT)

<a name="FeedbackAndSupport"></a>
## Feedback and Support

If you have any questions, comments and other support questions about how to
use this library, use the
[GitHub Discussions](https://github.com/bxparks/AceWire/discussions)
for this project. If you have bug reports or feature requests, file a ticket in
[GitHub Issues](https://github.com/bxparks/AceWire/issues). I'd love to hear
about how this software and its documentation can be improved. I can't promise
that I will incorporate everything, but I will give your ideas serious
consideration.

Please refrain from emailing me directly unless the content is sensitive. The
problem with email is that I cannot reference the email conversation when other
people ask similar questions later.

<a name="Authors"></a>
## Authors

Created by Brian T. Park (brian@xparks.net).
