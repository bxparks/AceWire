# AceWire

[![Validation](https://github.com/bxparks/AceWire/actions/workflows/validation.yml/badge.svg)](https://github.com/bxparks/AceWire/actions/workflows/validation.yml)

This library provides helper and wrapper classes to allow user applications to
use different I2C libraries through a small, consistent, and unified API on
different Arduino platforms. This library was originally part of the
[AceSegment](https://github.com/bxparks/AceSegment) library, but was extracted
into a separate library because it seemed useful as a separate library.

This library exists to work around a number of deficiencies in the standard
`<Wire.h>` library.

* The `TwoWire` class [cannot be used
  polymorphically](https://github.com/Testato/SoftwareWire/issues/28).
  In other words, the `TwoWire` class cannot be subclassed to implement
  different implementations.
* The `TwoWire` class inherits from the `Print` and `Stream` classes, which
  makes the surface area of the API far too large.
    * More frustratingly, the methods which are relevant to the I2C
      functionality are not part of the `Print` and `Stream` API.
    * But these I2C-related functions are *not* marked `virtual`, which prevents
      the `TwoWire` class from being a consistent API for different I2C
      implementations.
* Simply including `#include <Wire.h>` in a user application causes flash usage
  to increase by ~1100 bytes, and static memory to increase by ~110 bytes,
  even if nothing uses the `Wire` object.
    * This prevents a third party library from including device driver code for
      both SPI and I2C (for example), without inflating the flash usage for
      downstream applications which use only the SPI version.
* If an application written against the `<Wire.h>` API, it becomes difficult to
  switch to a different I2C library because various libraries have slightly
  different signatures and slightly semantics.
* The `TwoWire` class on many platforms (e.g. AVR) is hardcoded to use a 32-byte
  RX and TX buffer, which is insufficient for certain types of devices.
* The standard `<Wire.h>` library uses hardware interrupts which are supported
  on only certain GPIO pins (`SDA` and `SCL`).

Numerous [third party I2C
libraries](https://github.com/Testato/SoftwareWire/wiki/Arduino-I2C-libraries)
have been written to get around some of these problems. But the APIs for these
third party libraries often diverge in subtle ways from the `TwoWire` class,
and it can be difficult to write an application that is compatible with
different I2C libraries.

The AceWire library attempts to address these problems by making it easier to
use switch among the various third party I2C libraries so that the application
can decide to use the most appropriate implementation. The AceWire library
provides the following:

* Defines a small, unified [AceWire Interface](#AceWireInterface) to interact
  with various third party I2C libraries.
    * If applications can be written against the AceWire API, different I2C
      libraries can be swapped in with only small or no changes to the
      application code.
* Provides C++ template classes to wrap various third party libraries so that
  they can be used using the common API.
    * This AceWire does not directly depend on any other third party libraries,
      include `<Wire.h>`.
    * This means that third party device libraries can depend on AceWire,
      without pulling in `<Wire.h>`, which saves ~1100 bytes of flash if the
      downstream application never uses `<Wire.h>`.
* Does not use virtual functions to avoid runtime overhead.
    * The AceWire API is implemented as contracts, not through virtual
      functions, using compile-time polymorphism instead of runtime
      polymorphism.
    * This means that the compiler is able to optimize away all or almost all of
      the overhead of the indirection of the wrapper class.

The AceWire library provides wrapper interfaces for 8 of those third party
libraries:

* `TwoWireInterface` (all platforms)
    * Wrapper around the `TwoWire` class from the built-in `<Wire.h>` library.
* `FeliasFoggWireInterface` (all platforms)
    * Wrapper around the `SlowSoftWire` class from the
      https://github.com/felias-fogg/SlowSoftWire library.
* `MarpleWireInterface` (all platforms)
    * Wrapper around the `SoftWire` class from the
      https://github.com/stevemarple/SoftWire library.
* `RaemondWireInterface` (all platforms)
    * Wrapper around the `SoftWire` class from the
      https://github.com/RaemondBW/SWire library.
* `SeeedWireInterface` (all platforms)
    * Wrapper around the `SoftwareI2C` class from the
      https://github.com/Seeed-Studio/Arduino_Software_I2C library.
    * Not recommended due to the lack of any control over the I2C speed, and its
      use of actively driving the open-drain lines on the I2C bus.
* `TestatoWireInterface` (AVR only)
    * Wrapper around the `SoftwareWire` class from the
      https://github.com/Testato/SoftwareWire library.
* `ThexenoWireInterface` (AVR only)
    * Wrapper around the `TwoWire` class from the
      https://github.com/thexeno/HardWire-Arduino-Library library.
    * The `TwoWire` class name from this library is the same as the one from
      `<Wire.h>`.
* `TodbotWireInterface` (AVR only)
    * Wrapper around the `SoftI2CMaster` class from the
      https://github.com/todbot/SoftI2CMaster library.

The AceWire library also provides 2 classes which are not wrapper classes, but
are direct implementations of the AceWire API. According to
[examples/MemoryBenchmark](examples/MemoryBenchmark), these 2 implementations
consume the least amount of flash and static memory among the ones that I have
personally tested. I use these interface classes for many of my simpler projects
which do not require advanced I2C features:

* `SimpleWireInterface` (all platforms)
    * Software bitbanging implementation using `digitalWrite()` and
      `pinMode()`.
    * Capable of 50 kHz (AVR) to 200 kHz (Teensy 3.2) throughput.
* `SimpleWireFastInterface` (AVR only)
    * Same as `SimpleWireInterface.h` using the `digitalWriteFast()` and
      `pinModeFast()` from one of the `<digitalWriteFast.h>` libraries on AVR
      processors.
    * Can be **10X** faster than `SimpleWireInterface` on AVR processors.
    * Can reduce flash memory consumption by **10X** compared to `<Wire.h>`,
      about 250 bytes of flash compared to 2500 bytes.
    * Capable of 500-600 kHz throughput on AVR.

Once the user application implements the unified [AceWire
Interface](#AceWireInterface), it becomes easy to use alternative I2C libraries
instead of being locked into the pre-installed `<Wire.h>` library.

AceWire does have limitations. Its API is small and supports only a limited set
of I2C functionality, but these are often enough for many I2C devices:

* master mode only, no slave
* no multi-master negotiation
* no clock stretching
* only 7-bit addresses are supported, 10-bit addresses are not supported

**Version**: 0.4.0 (2022-01-26)

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
    * [AceWire Interface](#AceWireInterface)
        * [API methods](#ApiMethods)
        * [Omitted API methods](#OmittedApiMethods)
        * [Writing to I2C](#WritingToI2C)
        * [Reading from I2C](#ReadingFromI2C)
        * [Error Handling](#ErrorHandling)
    * [Interface Classes](#InterfaceClasses)
        * [TwoWireInterface](#TwoWireInterface)
        * [FeliasFoggWireInterface](#FeliasFoggWireInterface)
        * [MarpleWireInterface](#MarpleWireInterface)
        * [RaemondWireInterface](#RaemondWireInterface)
        * [SeeedWireInterface](#SeeedWireInterface)
        * [TestatoWireInterface](#TestatoWireInterface)
        * [ThexenoWireInterface](#ThexenoWireInterface)
        * [TodbotWireInterface](#TodbotWireInterface)
        * [SimpleWireInterface](#SimpleWireInterface)
        * [SimpleWireFastInterface](#SimpleWireFastInterface)
    * [Storing Interface Objects](#StoringInterfaceObjects)
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

The latest stable release is available from the Arduino Library Manager in the
IDE. Search for "AceWire". Click install.

The development version can be installed by cloning the
[GitHub repository](https://github.com/bxparks/AceWire), checking out the
default `develop` branch, then manually copying over to or symlinking from the
`./libraries` directory used by the Arduino IDE. (The result is a directory
or link named `./libraries/AceWire`.)

The `master` branch contains the stable releases.

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
    * [AceSegment/examples/Ht16k33Demo](https://github.com/bxparks/AceSegment/tree/develop/examples/Ht16k33Demo)

<a name="Usage"></a>
## Usage

<a name="HeaderAndNamespace"></a>
### Include Header and Namespace

The AceWire library needs only the `<AceWire.h>` header file. To prevent name
clashes with other libraries that the calling code may use, all classes are
defined in the `ace_wire` namespace. A specific `XxxWireInterface` class may
need to pull in other third party header files. This is a deliberate design to
allow the application to determine the direct dependency to its third party
libraries. The specifics are detailed below.

<a name="AceWireInterface"></a>
### AceWire Interface

The classes in this library provide the unified interface described by
the `XxxInterface` below. Downstream application classes can be coded against
this unified interface using C++ templates so that different implementations can
be selected at compile-time.

<a name="ApiMethods"></a>
#### API Methods

Different I2C libraries implement slightly different methods, with slightly
different semantics. Some implementations use internal TX and RX buffers (e.g.
`<Wire.h>`). Other implementations do not use any buffers. Some are hybrids,
using no buffer for RX but a buffer for TX. The unified interface attempts to
find a common API among these variations.

All `XxxInterface` wrapper classes in the AceWire library have the following
method signatures. The status codes of each method are normalized across third
party libraries so that they conform to the following conventions:

```C++
class XxxInterface {
  public:
    void begin() const;

    void end() const;

    /** Returns 0 upon success, 1 otherwise. */
    uint8_t beginTransmission(uint8_t addr) const;

    /** Returns 1 upon success, 0 otherwise. */
    uint8_t write(uint8_t data) const;

    /** Returns 0 upon success, non-zero code otherwise. */
    uint8_t endTransmission(bool sendStop = true) const;

    /** Returns quantity upon success, 0 otherwise. */
    uint8_t requestFrom(
        uint8_t addr, uint8_t quantity, bool sendStop = true) const;

    /** Returns the byte read from the bus. No error detection possible. */
    uint8_t read() const;
};
```

The `beginTransmission()` method returns a 0 upon success and 1 upon failure.
(This is slightly different than the `beginTransmission()` of the `TwoWire`
class which returns `void`.) For implementations using a TX buffer, this will
call the underlying `beginTransmission()`, then always return 0 to indicate
success because nothing is actually sent over the wire until the
`endTransmission()` is called. For implementations which do not use a TX buffer,
this method will send the `addr` byte on the I2C bus. If a device responds with
an ACK, then the method will return 0, otherwise it will return a 1 to indicate
failure.

The `write()` method returns 1 upon success, 0 upon failure. This is the
opposite convention of `beginTransmission()`, but it allows compatibility with
the native `<Wire.h>` library which returns a `size_t` representing the number
of bytes actually written. In this API, the return type is a `uint8_t` instead
of `size_t` because only a single byte can be transferred by `write()` at a
time. The `write()` method in the native `<Wire.h>` library always returns 1
because it simply writes the data byte into a buffer. But in non-buffered
implementations, the data is actually sent over the bus and this method returns
the ACK/NACK response of the slave device.

The `endTransmission()` method returns 0 upon success. The native `<Wire.h>`
library and a few other libraries return various error codes for different error
conditions. These error codes are poorly documented so third party I2C libraries
may not actually follow these codes:

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

Both the `endTransmission()` and `requestFrom()` methods accept an optional
`sendStop` flag which is `true` by default. This controls whether an I2C STOP
condition is sent after the completion of the transmission or reception of the
byte packet. Even though this parameter exists in the wrapper classes to provide
a consistent API, the actual functionality may not be implemented by the
underlying library and this parameter may be ignored.

<a name="OmittedApiMethods"></a>
#### Omitted API Methods

The following methods are often found in other I2C libraries, but have been
deliberately left out of AceWire:

* `bool available()`
* `size_t write(const uint8_t* buf, size_t n)`
* all other methods from the `Print` and `Stream` classes
* callback functions, e.g. `onReceive()` and `onRequest()`

The `available()` method is inherited by the `TwoWire` class from the `Stream`
class. But the semantics of the `available()` method cannot be implemented
within the I2C specification. When the master is reading from the slave, it is
the master that sends the ACK/NACK bit to the slave. There is no mechanism for
the slave to tell the master when no more bytes are available. If the slave
happens to stop sending early, then the SDA line will passively pull to HIGH,
and the master will read `0xFF`. In most I2C implementations, `available()`
simply returns the number of bytes requested (`quantity`) minus the number of
times `read()` was called. But there is at least one implementation which
instead returns the number of times that `read()` was called (which seems to be
a bug). I recommend users to ignore the `available()` method and always read
`quantity` number of bytes. For additional information, see
[ArduinoCore-avr#384](https://github.com/arduino/ArduinoCore-avr/issues/384) and
[ArduinoCore-avr#171](https://github.com/arduino/ArduinoCore-avr/issues/171).

The `write(buf, n)` method was left out to keep the AceWire API as small as
possible.  This method can be easily implemented by the calling code with a
simple loop. It was not clear to me how to handle error checking. Buffered
implementations would not need erorr checking on each call to `write(c)`, but
unbuffered implementations probably would. Making the client perform the loop
manually would allow them to decide how to handle errors.

All other methods from the `Print` and `Stream` classes are omitted because they
are overly complex and mostly irrelevant to the basic functionality of an I2C
library.

Some I2C libraries provide the ability to register callback functions that
trigger when bytes are received or sent. These are not supported by the AceWire
API because they are specific to each I2C implementation.

<a name="WritingToI2C"></a>
#### Writing to I2C

Once an instance of the `XxxInterface` class is created and passed into the
`T_WIREI mWireInterface` member variable in the `MyClass<T_WIREI>` class,
writing to the I2C device is basically the same as using the `Wire` object
directly:

```C++
#include <AceWire.h>

template <typename T_WIREI>
class MyClass {
  private:
    static const uint8_t ADDRESS = ...;
    ...

  public:
    explicit MyClass(T_WIREI& wireInterface)
        : mWireInterface(wireInterface)
    {...}

    void writeToDevice() {
      uint8_t status = mWireInterface.beginTransmission(ADDRESS);
      if (status) { /*error*/ }

      uint8_t n = mWireInterface.write(data1);
      if (n == 0) { /*error*/ }

      n = mWireInterface.write(data2);
      if (n == 0) { /*error*/ }
      ...

      status = mWireInterface.endTransmission();
      if (status) { /*error*/ }
    }

  private:
    T_WIREI mWireInterface; // copied by value
};

// Create an instance of the XxxInterface, which will usually be a wrapper
// around a concrete I2C implementation, in this example, the Wire instance of
// the TwoWire class.
TwoWireInterface<TwoWire> wireInterface(Wire);

// Create an instance of MyClass, passing an instance of the wireInterface
// object.
MyClass<TwoWireInterface> myClass(wireInterface);
```

Some I2C implementations (e.g. `Wire`, `SoftwareWire`) follow the design of
the `TwoWire` class from `<Wire.h>`, and use an internal buffer (often 32 bytes
on 8-bit platforms) to store the data. The `write(uint8_t)` method does not
actually write data to the bus, rather it writes into the buffer. The actual
transmission of the data does not occur until `endTransmission()` is called.
Even if the implementation uses hardware interrupts, the `endTransmission()`
method will be a blocking call that does not return until all the bytes in the
write buffer are sent.

Some I2C implementation do not use a write buffer (e.g. `SimpleWireInterface`,
`SimpleWireFastInterface`, and `SlowSoftWire` below.). Instead, the
`beginTransmission()`, `write()`, and `endTransmission()` methods directly write
the necessary data to the I2C bus.

For compatibility with both types of implementations, the calling code using
AceWire classes should assume that the write buffer does *not* exist. The
calling code should assume that the `beginTransmission()`, `write()`, and
`endTransmission()` are all blocking calls which send out the data to the I2C
immediately. Therefore, the `write()` method should be called as quickly as
possible.

<a name="ReadingFromI2C"></a>
#### Reading from I2C

Reading from the I2C bus is similar to writing. Once the `mWireInterface` object
is available in the `MyClass` object, we can read bytes from the I2C using the
`requestFrom()` and `read()` methods:

```C++
template <typename T_WIREI>
class MyClass {
  private:
    static const uint8_t ADDRESS = ...;
    static const uint8_t NUM_BYTES = ...;
    ...

  public:
    explicit MyClass(T_WIREI& wireInterface)
        : mWireInterface(wireInterface)
    {...}

    void readFromDevice() {
      uint8_t status = mWireInterface.requestFrom(ADDRESS, NUM_BYTES);
      if (status == 0) {
        /*error*/
      } else {
        uint8_t data1 = mWireInterface.read();
        uint8_t data2 = mWireInterface.read();
        ... // for total of 'NUM_BYTES' bytes
      }
    }

  private:
    T_WIREI mWireInterface; // copied by value
};

// Create an instance of the XxxInterface, which will usually be a wrapper
// around a concrete I2C implementation, in this example, the Wire instance of
// the TwoWire class.
TwoWireInterface<TwoWire> wireInterface(Wire);

// Create an instance of MyClass, passing an instance of the wireInterface
// object.
MyClass<TwoWireInterface> myClass(wireInterface);
```

In both buffered and unbuffered I2C library implementations, the
`requestFrom(addr, quantity)` method returns `quantity` upon success. If it
returns a 0 to indicate failure, the `read()` should *not* be called.

**Important**: For all implementations, the number of calls to the `read()`
method must be *exactly* equal to `quantity`. Otherwise, unpredictable things
may happen, including a crash of the system.

<a name="ErrorHandling"></a>
#### Error Handling

Performing proper error handling for the various I2C libraries is difficult.
Different I2C implementations handle errors in slightly different ways. Some
libraries and methods use 0 to indicate success, while other libraries use 1 to
indicate success for the same method name. The various XxxInterface classes
normalize the return value of various methods for each library to follow the
conventions described by [API Methods](#ApiMethods) above.

For some non-critical applications with simple hardware configurations using
only a few I2C devices, it may be acceptable to just ignore the error statuses,
and blindly read from and write from the I2C devices, assuming that the writing
or reading operations are always successful.

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

<a name="InterfaceClasses"></a>
### Interface Classes

This section contains concrete code fragments that show how to initialize and
use specific AceWire wrapper interface classes which are designed to work with
specific I2C libraries.

<a name="TwoWireInterface"></a>
#### TwoWireInterface

The `TwoWireInterface` is a thin wrapper around the `TwoWire` class provided by
the pre-installed `<Wire.h>` library. It is configured and used like this:

```C++
#include <Arduino.h>
#include <Wire.h> // TwoWire, Wire
#include <AceWire.h>
using ace_wire::TwoWireInterface;

template <typename T_WIREI>
class MyClass {
  private:
    static const uint8_t ADDRESS = ...;
    static const uint8_t NUM_BYTES = ...;

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
      mWireInterface.requestFrom(ADDRESS, NUM_BYTES);
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
  myClass.writeToDevice();
  myClass.readFromDevice();
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
    * represents one of the "interface" classes in *this* library: e.g.
      `TwoWireInterface`, `FeliasFoggWireInterface`, `SimpleWireInterface`,
      `SimpleWireFastInterface`, etc.

See the [Writing to I2C](#WritingToI2C) and [Reading from I2C](#ReadingFromI2C)
sections below for documentation about the `beginTransmission()`,
`endTransmission()`, and `requestFrom()`.

<a name="FeliasFoggWireInterface"></a>
#### FeliasFoggWireInterface

The `FeliasFoggWireInterface` is a thin wrapper around the `SlowSoftWire` class
provided by the https://github.com/felias-fogg/SlowSoftWire library.
It is a software implementation of I2C that is compatible across all platforms.
It is configured and used like this:

```C++
#include <Arduino.h>
#include <SlowSoftWire.h> // https://github.com/felias-fogg/SlowSoftWire
#include <AceWire.h>
using ace_wire::FeliasFoggWireInterface;

template <typename T_WIREI>
class MyClass {
  // same as above
};

const uint8_t SDA_PIN = 2;
const uint8_t SCL_PIN = 3;
SlowSoftWire slowSoftWire(SDA_PIN, SCL_PIN);
using WireInterface = FeliasFoggWireInterface<SlowSoftWire>;

WireInterface wireInterface(slowSoftWire);
MyClass<WireInterface> myClass(wireInterface);

void setup() {
  slowSoftWire.begin();
  wireInterface.begin();
  myClass.writeToDevice();
  myClass.readFromDevice();
  ...
}
```

<a name="MarpleWireInterface"></a>
#### MarpleWireInterface

The `MarpleWireInterface` is a thin wrapper around the `SoftWire` class
provided by the https://github.com/stevemarple/SoftWire library.
It is a software implementation of I2C that is compatible across all platforms.
It is configured and used like this:

```C++
#include <Arduino.h>
#include <SoftWire.h> // https://github.com/stevemarple/SoftWire
#include <AceWire.h>
using ace_wire::MarpleWireInterface;

template <typename T_WIREI>
class MyClass {
  // same as above
};

const uint8_t SDA_PIN = 2;
const uint8_t SCL_PIN = 3;
const uint8_t SOFT_WIRE_BUFFER_SIZE = 32;
uint8_t rxBuffer[SOFT_WIRE_BUFFER_SIZE];
uint8_t txBuffer[SOFT_WIRE_BUFFER_SIZE];

SoftWire softWire(SDA_PIN, SCL_PIN);
using WireInterface = MarpleWireInterface<SoftWire>;
WireInterface wireInterface(softWire);

MyClass<WireInterface> myClass(wireInterface);

void setup() {
  softWire.setRxBuffer(rxBuffer, SOFT_WIRE_BUFFER_SIZE);
  softWire.setTxBuffer(txBuffer, SOFT_WIRE_BUFFER_SIZE);
  softWire.begin();
  wireInterface.begin();
  myClass.writeToDevice();
  myClass.readFromDevice();
  ...
}
```

<a name="RaemondWireInterface"></a>
#### RaemondWireInterface

The `RaemondWireInterface` is a thin wrapper around the `SoftWire` class
provided by the https://github.com/RaemondBW/SWire library.
It is a software implementation of I2C that is compatible across all platforms.
It is configured and used like this:

```C++
#include <Arduino.h>
#include <SWire.h> // https://github.com/RaemondBW/SWire
#include <AceWire.h>
using ace_wire::RaemondWireInterface;

template <typename T_WIREI>
class MyClass {
  // same as above
};

const uint8_t SDA_PIN = 2;
const uint8_t SCL_PIN = 3;
using WireInterface = RaemondWireInterface<SoftWire>;
WireInterface wireInterface(SWire);

MyClass<WireInterface> myClass(wireInterface);

void setup() {
  SWire.begin(SDA_PIN, SCL_PIN);
  wireInterface.begin();
  myClass.writeToDevice();
  myClass.readFromDevice();
  ...
}
```

<a name="SeeedWireInterface"></a>
#### SeeedWireInterface

The `SeeedWireInterface` is a thin wrapper around the `SoftwareI2C` class
provided by the https://github.com/Seeed-Studio/Arduino_Software_I2C library.
It is a software implementation of I2C that is compatible across all platforms.
However, it provides no `delayMicroseconds()` between bit transitions, so on
very fast processors (e.g. ESP32), the frequency of the I2C bus can exceed the
speed limit of the slave device, causing communication failures. In addition,
this library is the only one I'm aware of that actively pulls up the I2C lines
(using `digitalWrite(pin, HIGH)`) which can cause problems with the open-drain
I2C bus. I do not recommend using this library, but it it is configured and used
like this:

```C++
#include <Arduino.h>
#include <SoftwareI2C.h> // https://github.com/Seeed-Studio/Arduino_Software_I2C
#include <AceWire.h>
using ace_wire::SeeedWireInterface;

template <typename T_WIREI>
class MyClass {
  // same as above
};

const uint8_t SDA_PIN = 2;
const uint8_t SCL_PIN = 3;
SoftwareI2C seeedWire;
using WireInterface = SeeedSoftwareI2C<SoftwareI2C>;
WireInterface wireInterface(seeedWire);

MyClass<WireInterface> myClass(wireInterface);

void setup() {
  seeedWire.begin(SDA_PIN, SCL_PIN);
  wireInterface.begin();
  myClass.writeToDevice();
  myClass.readFromDevice();
  ...
}
```

<a name="TestatoWireInterface"></a>
#### TestatoWireInterface

The `TestatoWireInterface` is a thin wrapper around the `SoftwareWire` class
provided by the https://github.com/RaemondBW/SWire library.
It is a software implementation of I2C that uses direct access to GPIO ports so
is compatible with AVR processors only.
It is configured and used like this:

```C++
#include <Arduino.h>
#include <SoftwareWire.h> // https://github.com/Testato/SoftwareWire (AVR only)
#include <AceWire.h>
using ace_wire::TestatoWireInterface;

template <typename T_WIREI>
class MyClass {
  // same as above
};

const uint8_t SDA_PIN = 2;
const uint8_t SCL_PIN = 3;
SoftwareWire softwareWire(SDA_PIN, SCL_PIN);
using WireInterface = TestatoWireInterface<SoftwareWire>;
WireInterface wireInterface(softwareWire);

MyClass<WireInterface> myClass(wireInterface);

void setup() {
  softwareWire.begin();
  wireInterface.begin();
  myClass.writeToDevice();
  myClass.readFromDevice();
  ...
}
```

<a name="ThexenoWireInterface"></a>
#### ThexenoWireInterface

The `ThexenoWireInterface` is a thin wrapper around the `TwoWire` class
provided by the https://github.com/thexeno/HardWire-Arduino-Library library.
It is a hardware implementation of I2C that is a fork of the AVR version of
`<Wire.h>`, so is compatible with AVR processors only.
It is configured and used like this:

```C++
#include <Arduino.h>
#include <HardWire.h> // https://github.com/thexeno/HardWire-Arduino-Library
#include <AceWire.h>
using ace_wire::ThexenoWireInterface;

template <typename T_WIREI>
class MyClass {
  // same as above
};

using WireInterface = ThexenoWireInterface<TwoWire>;
WireInterface wireInterface(Wire);

MyClass<WireInterface> myClass(wireInterface);

void setup() {
  Wire.begin();
  wireInterface.begin();
  myClass.writeToDevice();
  myClass.readFromDevice();
  ...
}
```

<a name="TodbotWireInterface"></a>
#### TodbotWireInterface

The `TodbotWireInterface` is a thin wrapper around the `SoftI2CMaster` class
provided by the https://github.com/todbot/SoftI2CMaster library.
It is a software implementation of I2C but uses direct GPIO port access for AVR
processors, so is compatible only with AVR processors. Unfortunately, the
`SoftI2CMaster` class inherits from the `TwoWire` class of `<Wire.h>`, so it
pulls in all the code for the hardware I2C implementation, including the RX and
TX buffers from `TwoWire`, then overrides the code with its own software
implementation. This probably means that it incorporates the deficiencies of
both worlds.

Another interesting quirk of this library, it provides a separate
`read()` method and `readLast()` method, where the `readLast()` method needs to
be used for the last byte. All other I2C libraries combine the 2 into a single
`read()` method that automatically does the correct operation for the last byte.
The `TodbotWireInterface` interface class could correct for this deficiency, but
I didn't want to spend time writing and debugging this logic for a library that
I don't use.

I do not recommend using this library, but it is configured like this:

```C++
#include <Arduino.h>
#include <SoftwareI2CMaster.h> // https://github.com/todbot/SoftI2CMaster
#include <AceWire.h>
using ace_wire::TodbotWireInterface;

template <typename T_WIREI>
class MyClass {
  // same as above
};

const uint8_t SDA_PIN = 2;
const uint8_t SCL_PIN = 3;
SoftwareI2CMaster todbotWire(SCL_PIN, SDA_PIN);
using WireInterface = TodbotWireInterface<SoftI2CMaster>;
WireInterface wireInterface(todbotWire);

MyClass<WireInterface> myClass(wireInterface);

void setup() {
  todbotWire.begin();
  wireInterface.begin();
  myClass.writeToDevice();
  myClass.readFromDevice();
  ...
}
```

<a name="SimpleWireInterface"></a>
#### SimpleWireInterface

The `SimpleWireInterface` is a software implementation of I2C using
`digitalWrite()` and `pinMode()` to implement the AceWire API directly. It is
configured and used like this:

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
#### SimpleWireFastInterface

The `SimpleWireFastInterface` is the same as `SimpleWireInterface` but using the
`digitalWriteFast()` and `pinModeFast()` functions. It is configured and used
like this:

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

The internal size of the `XxxWireInterface` object is just a single reference to
the `T_WIREI` object, so there is no difference in the static memory size.
However, storing the `mWireInterface` as a reference causes an unnecessary extra
layer of indirection every time the `mWireInterface` object is called. In almost
every case, I recommend storing the `XxxInterface` object by value into the
`MyClass` object.

<a name="ResourceConsumption"></a>
## Resource Consumption

<a name="FlashAndStaticMemory"></a>
### Flash And Static Memory

The Memory benchmark numbers can be seen in
[examples/MemoryBenchmark](examples/MemoryBenchmark). Here are 2 samples:

**Arduino Nano**

```
+--------------------------------------------------------------------+
| functionality                         |  flash/  ram |       delta |
|---------------------------------------+--------------+-------------|
| baseline                              |    456/   11 |     0/    0 |
|---------------------------------------+--------------+-------------|
| TwoWireInterface<TwoWire>             |   2926/  229 |  2470/  218 |
| SimpleWireInterface                   |   1336/   16 |   880/    5 |
| SimpleWireFastInterface               |    714/   13 |   258/    2 |
|---------------------------------------+--------------+-------------|
| FeliasFoggWireInterface<SlowSoftWire> |   2008/   83 |  1552/   72 |
| MarpleWireInterface<SoftWire>         |   2936/  135 |  2480/  124 |
| RaemondWireInterface<SWire>           |   1674/   85 |  1218/   74 |
| SeeedWireInterface<SoftwareI2C>       |   1626/   21 |  1170/   10 |
|---------------------------------------+--------------+-------------|
| TestatoWireInterface<SoftwareWire>    |   2454/   72 |  1998/   61 |
| ThexenoWireInterface<TwoWire>         |   2264/  477 |  1808/  466 |
+--------------------------------------------------------------------+
```

**ESP8266**

```
+--------------------------------------------------------------------+
| functionality                         |  flash/  ram |       delta |
|---------------------------------------+--------------+-------------|
| baseline                              | 260089/27892 |     0/    0 |
|---------------------------------------+--------------+-------------|
| TwoWireInterface<TwoWire>             | 264485/28384 |  4396/  492 |
| SimpleWireInterface                   | 261521/28000 |  1432/  108 |
|---------------------------------------+--------------+-------------|
| FeliasFoggWireInterface<SlowSoftWire> | 263513/28056 |  3424/  164 |
| MarpleWireInterface<SoftWire>         | 263753/28128 |  3664/  236 |
| RaemondWireInterface<SWire>           | 262133/28076 |  2044/  184 |
| SeeedWireInterface<SoftwareI2C>       | 261693/28008 |  1604/  116 |
+--------------------------------------------------------------------+
```

<a name="CpuCycles"></a>
### CPU Cycles

The CPU benchmark numbers can be seen in
[examples/AutoBenchmark](examples/AutoBenchmark). Here are 2 samples:

**Arduino Nano**

```
+-------------------------------------------+-------------------+----------+
| Functionality                             |   min/  avg/  max | eff kbps |
|-------------------------------------------+-------------------+----------|
| TwoWireInterface<TwoWire>,100kHz          |   932/  936/  948 |     86.5 |
| TwoWireInterface<TwoWire>,400kHz          |   312/  321/  332 |    252.3 |
| SimpleWireInterface,1us                   |  1644/ 1664/ 1824 |     48.7 |
| SimpleWireFastInterface,1us               |   140/  152/  160 |    532.9 |
|-------------------------------------------+-------------------+----------|
| RaemondWireInterface<SoftWire>            |  2504/ 2526/ 2772 |     32.1 |
| FeliasFoggWireInterface<SlowSoftWire>     |  1852/ 1871/ 2048 |     43.3 |
| SeeedWireInterface<SoftwareI2C>           |  1912/ 1936/ 2116 |     41.8 |
|-------------------------------------------+-------------------+----------|
| TestatoWireInterface<SoftwareWire>,100kHz |  1368/ 1374/ 1432 |     59.0 |
| TestatoWireInterface<SoftwareWire>,400kHz |   980/  995/ 1088 |     81.4 |
+-------------------------------------------+-------------------+----------+
```

**ESP8266**

```
+-------------------------------------------+-------------------+----------+
| Functionality                             |   min/  avg/  max | eff kbps |
|-------------------------------------------+-------------------+----------|
| TwoWireInterface<TwoWire>,100kHz          |  1105/ 1113/ 1259 |     72.8 |
| TwoWireInterface<TwoWire>,400kHz          |   229/  229/  232 |    353.7 |
| SimpleWireInterface,1us                   |   816/  818/  860 |     99.0 |
|-------------------------------------------+-------------------+----------|
| RaemondWireInterface<SoftWire>            |   988/  991/ 1059 |     81.7 |
| FeliasFoggWireInterface<SlowSoftWire>     |   828/  831/  895 |     97.5 |
| SeeedWireInterface<SoftwareI2C>           |   598/  601/  650 |    134.8 |
+-------------------------------------------+-------------------+----------+
```

<a name="SystemRequirements"></a>
## System Requirements

<a name="Hardware"></a>
### Hardware

**Tier 1: Fully Supported**


* Arduino Nano (16 MHz ATmega328P)
* SparkFun Pro Micro (16 MHz ATmega32U4)
* SAMD21 M0 Mini (48 MHz ARM Cortex-M0+)
* STM32 Blue Pill (STM32F103C8, 72 MHz ARM Cortex-M3)
* NodeMCU 1.0 (ESP-12E module, 80MHz ESP8266)
* WeMos D1 Mini (ESP-12E module, 80 MHz ESP8266)
* ESP32 dev board (ESP-WROOM-32 module, 240 MHz dual core Tensilica LX6)
* Teensy 3.2 (72 MHz ARM Cortex-M4)

**Tier 2: Should Work**

These boards should work but I don't test these as often:

* ATtiny85 (8 MHz ATtiny85)
* Arduino Pro Mini (16 MHz ATmega328P)
* Teensy LC (48 MHz ARM Cortex-M0+)
* Mini Mega 2560 (Arduino Mega 2560 compatible, 16 MHz ATmega2560)

**Tier 3: May work, but not supported**

* SAMD21 M0 Mini (48 MHz ARM Cortex-M0+)
    * Arduino-branded SAMD21 boards use the ArduinoCore-API, so are explicitly
      blacklisted. See below.
    * Other 3rd party SAMD21 boards *may* work using the SparkFun SAMD core.
    * However, as of SparkFun SAMD Core v1.8.6 and Arduino IDE 1.8.19, I can no
      longer upload binaries to these 3rd party boards due to errors.
    * Therefore, third party SAMD21 boards are now in this new Tier 3 category.
    * This library may work on these boards, but I can no longer support them.

**Tier Blacklisted**

The following boards are *not* supported and are explicitly blacklisted to allow
the compiler to print useful error messages instead of hundreds of lines of
compiler errors:

* Any platform using the ArduinoCore-API
  (https://github.com/arduino/ArduinoCore-api). For example:
    * Nano Every
    * MKRZero
    * Raspberry Pi Pico RP2040

<a name="ToolChain"></a>
### Tool Chain

* [Arduino IDE 1.8.19](https://www.arduino.cc/en/Main/Software)
* [Arduino CLI 0.19.2](https://arduino.github.io/arduino-cli)
* [SpenceKonde ATTinyCore 1.5.2](https://github.com/SpenceKonde/ATTinyCore)
* [Arduino AVR Boards 1.8.4](https://github.com/arduino/ArduinoCore-avr)
* [Arduino SAMD Boards 1.8.9](https://github.com/arduino/ArduinoCore-samd)
* [SparkFun AVR Boards 1.1.13](https://github.com/sparkfun/Arduino_Boards)
* [SparkFun SAMD Boards 1.8.6](https://github.com/sparkfun/Arduino_Boards)
* [STM32duino 2.2.0](https://github.com/stm32duino/Arduino_Core_STM32)
* [ESP8266 Arduino 3.0.2](https://github.com/esp8266/Arduino)
* [ESP32 Arduino 2.0.2](https://github.com/espressif/arduino-esp32)
* [Teensyduino 1.56](https://www.pjrc.com/teensy/td_download.html)

This library is *not* compatible with:

* Any platform using the
  [ArduinoCore-API](https://github.com/arduino/ArduinoCore-api), for example:
    * [Arduino megaAVR](https://github.com/arduino/ArduinoCore-megaavr/)
    * [MegaCoreX](https://github.com/MCUdude/MegaCoreX)
    * [Arduino SAMD Boards >=1.8.10](https://github.com/arduino/ArduinoCore-samd)

It should work with [PlatformIO](https://platformio.org/) but I have
not tested it.

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

If you have any questions, comments, or feature requests for this library,
please use the [GitHub
Discussions](https://github.com/bxparks/AceWire/discussions) for this project.
If you have bug reports, please file a ticket in [GitHub
Issues](https://github.com/bxparks/AceWire/issues). Feature requests should go
into Discussions first because they often have alternative solutions which are
useful to remain visible, instead of disappearing from the default view of the
Issue tracker after the ticket is closed.

Please refrain from emailing me directly unless the content is sensitive. The
problem with email is that I cannot reference the email conversation when other
people ask similar questions later.

<a name="Authors"></a>
## Authors

Created by Brian T. Park (brian@xparks.net).
