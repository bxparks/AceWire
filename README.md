# AceWire

[![Validation](https://github.com/bxparks/AceWire/actions/workflows/validation.yml/badge.svg)](https://github.com/bxparks/AceWire/actions/workflows/validation.yml)

Wrapper classes that provide a simple, unified interface for different I2C
implementations on Arduino platforms. The code was initially part of the
[AceSegment](https://github.com/bxparks/AceSegment) library, but was extracted
into a separate library.

The library provides three I2C classes:

* `TwoWireInterface.h`
    * A thin, C++ template, wrapper class around an underlying I2C
      implementation library which provides a unified interface to client
      applications.
    * Compatible with the pre-installed `<Wire.h>` library, and other third
      party I2C libraries which expose an API that is syntactically compatible
      with the `TwoWire` class. At least 5 different third party libraries have
      been verified to work with `TwoWireInterface`:
        * [Using Third Party I2C Libraries](#UsingThirdPartyI2CLibraries)
        * [Third Party Compatibility](#ThirdPartyCompatibility)
* `SimpleWireInterface.h`
    * AceWire's own software bitbanging implementation that supports writing
      and reading from simple I2C devices, such as an HT16K33 LED controller
      chip and a DS3231 RTC chip.
    * It implements the `TwoWireInterface` directly using `digitalWrite()` and
      `pinMode().
* `SimpleWireFastInterface.h`
    * Same as `SimpleWireInterface.h` using one of the `<digitalWriteFast.h>`
      libraries that's available on AVR processors.
    * Can be up to 4X faster than `SimpleWireInterface` on AVR processors.
    * Can reduce flash memory consumption by **10X** compared to `<Wire.h>`,
      about 250 bytes of flash compared to 2500 bytes.

All 3 classes in this library implement the same API, but they do not use object
inheritance nor virtual methods. They are meant to be used as template arguments
to the end-user's template class. This design was motivated by the fact that the
`TwoWire` class in the pre-installed `<Wire.h>` library cannot be used
polymorphically. In other words, subclasses cannot be used through a pointer (or
reference) to the base `TwoWire` class. This library uses C++ templates to
provide compile-time polymorphism instead of runtime polymorphism. This also
means that the calling application code pays only a minimal runtime overhead for
the abstraction by avoiding the `virtual` dispatch.

Once the client application implements the API exposed by `TwoWireInterface`,
it becomes easy to use alternative I2C libraries instead of being locked into
the pre-installed `<Wire.h>` library. Alternative libraries may be desirable for
various reasons: To use different GPIO pins, instead of the hardware `SDA` and
`SCL`; to reduce flash and static memory consumption compared to `<Wire.h>`; to
use packet sizes larger than the default 32-bytes provided by AVR `<Wire.h>`.

Another advantage of using `<AceWire.h>` is to avoid direct dependency to the
`<Wire.h>` library. For reasons that are not obvious, simply including the
`<Wire.h>` header file causes the memory consumption of the application to
increase by 1140 bytes of flash and 113 bytes of static memory, **even** if the
application does not use anything from the `<Wire.h>` library. Using the
`<AceWire.h>` intermediary is particularly useful within another library.
Instead of depending on `<Wire.h>` directly, the library can depend on
`<AceWire.h>` instead. If the client application uses something from that
library that does not need `<Wire.h>`,then the client can still compile and run,
without suffering from unnecessary memory bloat.

The library currently supports only a limited set of I2C functionality:

* master mode only, no slave
* no multi-master negotiation
* no clock stretching
* able to use any GPIO pin
* tested AVR, SAMD21, STM32, ESP8266, ESP32, and Teensy 3.2
* repeated start seems to work
* only 7-bit addresses are supported, 10-bit addresses are not supported

**Version**: 0.3.2 (2021-08-17)

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
    * [examples/ReadWriteDemo](examples/ReadWriteDemo)
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
};
```

The `beginTransmission()` method returns a 0 upon success and 1 uon failure.
(This is slightly different than the `beginTransmission()` of the `TwoWire`
class which returns `void`.) For implementations using a transmit buffer, this
will always return a success because nothing is actually sent over the wire
until the `endTransmission()`. For implementations which do not use a buffer,
this will directly send the `addr` byte on the I2C bus. If a device responds
with an ACK, then the method will return 0, otherwise it will return a 1 to
indicate failure.

The `write()` method returns 1 upon success, 0 upon failure. This is the
opposite convention of `beginTransmission()`, but it allows compatibility with
the native `<Wire.h>` library which returns a `size_t` representing the number
of bytes actually written. In this API, the return type is a `uint8_t` instead
of `size_t`, but this is sufficient because only a single byte can be
transferred by `write()` at a time. The `write()` method in the native
`<Wire.h>` library always returns 1 because it simply writes the data byte into
a buffer. But in non-buffered implementations, the data is actually sent over
the wire and this method returns the ACK/NACK response of the slave device.

The `endTransmission()` method returns 0 upon success. The native `<Wire.h>`
library and a few other libraries return other non-zero error codes depending on
the error condition. These error codes are poorly documented so third party
I2C libraries may not actually follow these codes:

* 0: success
* 1: length too long for buffer
* 2: address send, NACK received
* 3: data send, NACK received
* 4: other twi error (lost bus arbitration, bus error, ..)

On non-buffered implementations, the `endTransmission()` method always returns a
0 because all of the data transmission already happened in the `write()` method
and the `endTransmission()` method simply sends a STOP condition if requested.

The `requestFrom()` method returns `quantity` if successful, or 0 if an error
occurs. In buffered I2C libraries, all of the data transfer happens in this
method so a return value of `quantity` means the transfer was successful. But in
unbuffered I2C libraries, this method simply sends the `addr` byte, so a 0 means
that the slave device responded with a NACK.

The `read()` method returns the data byte from the slave. There is no ability to
detect an error condition from the slave in this method, mostly because it is
the master that sends the ACK or NACK bit to the slave. In both buffered and
unbuffered implementations, the `read()` should NOT be called if the
`requestFrom()` method returns failure (i.e. 0).

Notice that the classes in this library do *not* inherit from a common interface
with virtual functions. This saves several hundred bytes of flash memory on
8-bit AVR processors by avoiding the dynamic dispatch. Often the
compiler is able to optimize away the overhead of calling the methods in this
library so that the function call is made directly to the underlying
implementation. The reduction of flash memory consumption is especially large
for classes that use the digitalWriteFast libraries which use compile-time
constants for pin numbers.

Also note that unlike many I2C libraries including `<Wire.h>`, AceWire does
**not** provide an `available()` method. This is because the functionality of
that method cannot be implemented within the I2C specification. When the master
is reading from the slave, it is the master that sends the ACK/NACK bit to the
slave. There is no mechanism for the slave to tell the master when no more
bytes are available. If the slave happens to stop sending early, then the SDA
line will passively pull to HIGH, and the master will read `0xFF`. In most I2C
implementations, `available()` simply returns the number of bytes requested
(`quantity`) minus the number of times `read()` was called. But there is at
least one implementation which instead returns the number of times that `read()`
was called (which seems to be a bug). I recommend users to ignore the
`available()` method and always read `quantity` number of bytes. For additional
information, see
[ArduinoCore-avr#384](https://github.com/arduino/ArduinoCore-avr/issues/384) and
[ArduinoCore-avr#171](https://github.com/arduino/ArduinoCore-avr/issues/171).

Two overloaded versions of `endTransmission()` and `requestFrom()` are defined
in `TwoWireInterface`, instead of defining a default value with a `bool sendStop
= bool` parameter. Having 2 separate versions of `endTransmission()` and
`requestFrom()` allows `TwoWireInterface` to be used with third party I2C
libraries which do not provide a version of `endTransmission()` or
`requestFrom()` methods with the `sendStop` parameter. (The `TwoWireInterface`
class will still compile as long as the non-existent third party method is not
used at all by the calling code.)

<a name="TwoWireInterface"></a>
### TwoWireInterface

The `TwoWireInterface` is a thin wrapper around the `TwoWire` class provided by
the pre-installed `<Wire.h>` library. It looks like this:

```C++
namespace ace_wire {

template <typename T_WIRE>
class TwoWireInterface {
  public:
    explicit TwoWireInterface(T_WIRE& wire) : mWire(wire) {}

    void begin() {...}
    void end() {...}

    uint8_t beginTransmission(uint8_t addr) {...}
    uint8_t write(uint8_t data) {...}
    uint8_t endTransmission(bool sendStop) {...}
    uint8_t endTransmission() {...}

    uint8_t requestFrom(uint8_t addr, uint8_t quantity, bool sendStop) {...}
    uint8_t requestFrom(uint8_t addr, uint8_t quantity) {...}
    uint8_t read() {...}
};

}
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

template <typename T_WIREI>
class MyClass {
  private:
    static const uint8_t ADDRESS = ...;
    static const uint8_t NUM_BYTES = ...;
    static const bool SEND_STOP = true;

  public:
    explicit MyClass(T_WIREI& wireInterface)
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
    }

  private:
    T_WIREI mWireInterface; // copied by value
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

Both the `T_WIRE` and `T_WIREI` template parameters contain a `T_` prefix to
avoid name collisions with the numerous `#define` macros defined in the global
namespace on Arduino platforms. The difference between `T_WIRE` and `T_WIREI`
may be a bit confusing so I hope the following explanation helps:

* `T_WIRE`
    * template parameter of the `TwoWireInterface` template class
    * represents a class that looks and acts like the `TwoWire` class from the
      built-in `<Wire.h>` library
    * can be a class from a third party library
* `T_WIREI`
    * template parameter of a user-defined class from the calling application
    * represents one of the "interface" classes in *this* library:
      `TwoWireInterface`, `SimpleWireInterface`, or `SimpleWireFastInterface`

See the [Writing to I2C](#WritingToI2C) and [Reading from I2C](#ReadingFromI2C)
sections below for documentation about the `beginTransmission()`,
`endTransmission()`, and `requestFrom()`.

<a name="SimpleWireInterface"></a>
### SimpleWireInterface

The `SimpleWireInterface` is a software implementation of I2C that has just
enough functionality to communicate with some simple I2C devices, such as an
HT16K33 LED controller chip, or a DS3231 RTC chip. It currently supports just a
master mode, with no clock stretching. It looks like this:

```C++
namespace ace_wire {

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
};

}
```
It is configured and used by the calling code `MyClass` like this:

```C++
#include <Arduino.h>
#include <AceWire.h>
using ace_wire::SimpleWireInterface;

template <typename T_WIREI>
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
namespace ace_wire {

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
};

}
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

template <typename T_WIREI>
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

In the above examples, the `MyClass` object holds the `T_WIREI` interface object
**by value**. In other words, the interface object is copied into the `MyClass`
object. This is efficient because interface objects are very small in size, and
copying them by-value avoids an extra level of indirection when they are used
inside the `MyClass` object. The compiler will generate code that is equivalent
to calling the underlying `Wire` methods through the `TwoWire` pointer.

The alternative is to save the `T_WIREI` object **by reference** like this:

```C++
template <typename T_WIREI>
class MyClass {
  public:
    explicit MyClass(T_WIREI& wireInterface)
        : mWireInterface(wireInterface)
    {...}

    [...]

  private:
    T_WIREI& mWireInterface; // copied by reference
};
```

The internal size of the `TwoWireInterface` object is just a single reference to
the `T_WIREI` object, so there is no difference in the static memory size.
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

template <typename T_WIREI>
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

The critical point is that `MyClass<T_WIREI>` remains unchanged from the
previous example, and everything should just work.

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

Once the `T_WIREI mWireInstnace` is created in the `MyClass<T_WIREI>` class,
writing to the I2C device is basically the same as using the `Wire` object
directly:

```C++
template <typename T_WIREI>
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
    T_WIREI mWireInterface; // copied by value
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
template <typename T_WIREI>
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
    }

  private:
    T_WIREI mWireInterface; // copied by value
};
```

The `requestFrom()` and `read()` methods should look familiar to those who have
used the `TwoWire` class from `<Wire.h>`.

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
some non-critical applications with simple hardware configurations, with only a
few I2C devices on the bus, it may be acceptable to just ignore the error
statuses from the underlying library, and just assume that the writing or
reading operations are always successful.

Some error checking is possible by checking the return values of some of the
methods:

* The `beginTransmission()` method in the native `<Wire.h>` library returns a
  `void`. In this library, it returns a `uint8_t` where a `0` indicates success.
  The implementation in `TwoWireInterface` always returns a 0 because it assumes
  that it is a wrapper around the native `<Wire.h>` library. But the software
  implementations of `SimpleWireInterface` and `SimpleWireFastInterface` do not
  use buffers, so `beginTransmission()` returns the ACK/NACk response from the
  device after it has been sent the address byte.
* The `write()` method returns a 1 on success and a 0 on failure. This is the
  reverse of the status code for `endTransmission()` because the
  implementation of `write()` in the native `TwoWire` class returns the number
  of bytes that was transferred by this method, not the value of the ACK/NACK
  response from the slave device.
* The `endTransmission()` returns 0 upon success, and a non-zero error code
  upon failure.
* The `requestFrom()` method returns `quantity` upon success, and 0 upon
  failure.
* The `read()` method cannot return an error code because the I2C protocol does
  not allow the slave device from sending an ACK/NACK to the master during the
  reading operation. It is the master that sends the ACK/NACK to the slave
  device. In many software implementations, the number of calls to `read()`
  must be exactly `quantity`, even if an error condition is raised in the middle
  of the transaction, because the I2C STOP condition is issued by the last
  `read()` operation.

See [examples/ReadWriteDemo](examples/ReadWriteDemo) for examples of some error
detection and handling.

Keep in mind that different I2C libraries may trigger error conditions at
different points in their workflow. Quite often, it depends on whether the
implementation is buffered or unbuffered. You may need to customize the error
handling for different I2C libraries.

The software I2C implementations provided in this library (`SimpleWireInterface`
and `SimpleWireFastInterface`) do not implement clock stretching, and they do
not check to see if another I2C master is controlling the bus. Therefore, they
cannot become wedged into an infinite loop so they do not provide a timeout
parameter.

The native `<Wire.h>` has the potential for becoming wedged. Recently, some work
was been done to allow the library to time out after a certain amount of time.
But I am not very familiar with those latest feature additions.

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
| SimpleWireInterface                 |   1336/   16 |   880/    5 |
| SimpleWireFastInterface             |    714/   13 |   258/    2 |
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
| SimpleWireInterface                 | 257780/26804 |  1080/   20 |
|-------------------------------------+--------------+-------------|
| TwoWireInterface<SWire>             | 258408/26944 |  1708/  160 |
| TwoWireInterface<SlowSoftWire>      | 259972/26884 |  3272/  100 |
| TwoWireInterface<SeeedSoftwareI2C>  | 257904/26812 |  1204/   28 |
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
| TwoWireInterface<TwoWire>,100kHz        |   932/  936/  948 |     86.5 |
| TwoWireInterface<TwoWire>,400kHz        |   312/  321/  332 |    252.3 |
| SimpleWireInterface,1us                 |  1644/ 1664/ 1824 |     48.7 |
| SimpleWireFastInterface,1us             |   140/  152/  160 |    532.9 |
|-----------------------------------------+-------------------+----------|
| TwoWireInterface<SoftwareWire>,100kHz   |  1368/ 1374/ 1432 |     59.0 |
| TwoWireInterface<SoftwareWire>,400kHz   |   984/  994/ 1088 |     81.5 |
| TwoWireInterface<SWire>                 |  2504/ 2524/ 2768 |     32.1 |
| TwoWireInterface<SlowSoftWire>          |  1852/ 1869/ 2048 |     43.3 |
| TwoWireInterface<SeeedSoftwareI2C>      |  1468/ 1485/ 1620 |     54.5 |
+-----------------------------------------+-------------------+----------+
```

**ESP8266**

```
+-----------------------------------------+-------------------+----------+
| Functionality                           |   min/  avg/  max | eff kbps |
|-----------------------------------------+-------------------+----------|
| TwoWireInterface<TwoWire>,100kHz        |   849/  856/ 1003 |     94.6 |
| TwoWireInterface<TwoWire>,400kHz        |   222/  222/  223 |    364.9 |
| SimpleWireInterface,1us                 |   853/  855/  901 |     94.7 |
|-----------------------------------------+-------------------+----------|
| TwoWireInterface<SWire>                 |   928/  934/ 1011 |     86.7 |
| TwoWireInterface<SlowSoftWire>          |   866/  870/  945 |     93.1 |
| TwoWireInterface<SeeedSoftwareI2C>      |   314/  317/  378 |    255.5 |
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
