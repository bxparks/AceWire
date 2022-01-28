# AutoBenchmark

This program determines the speed of various I2C implementations supported by
the AceWire library. To obtain accurate results, an actual I2C device must exist
on the bus. Currently, I use a DS3231 RTC chip because it is relatively small
and inexpensive and I can install one on each of my development boards.

**Version**: AceWire v0.4.1

**DO NOT EDIT**: This file was auto-generated using `make README.md`.

## Dependencies

This program depends on the following libraries:

* [AceCommon](https://github.com/bxparks/AceCommon)

On AVR processors, one of the following library is required to run the
`digitalWriteFast()` versions of the low-level drivers:

* https://github.com/watterott/Arduino-Libs/tree/master/digitalWriteFast
* https://github.com/NicksonYap/digitalWriteFast

## How to Generate

This requires the [AUniter](https://github.com/bxparks/AUniter) script
to execute the Arduino IDE programmatically.

The `Makefile` has rules to generate the `*.txt` results file for several
microcontrollers that I usually support, but the `$ make benchmarks` command
does not work very well because the USB port of the microcontroller is a
dynamically changing parameter. I created a semi-automated way of collect the
`*.txt` files:

1. Connect the microcontroller to the serial port. I usually do this through a
USB hub with individually controlled switch.
2. Type `$ auniter ports` to determine its `/dev/ttyXXX` port number (e.g.
`/dev/ttyUSB0` or `/dev/ttyACM0`).
3. If the port is `USB0` or `ACM0`, type `$ make nano.txt`, etc.
4. Switch off the old microontroller.
5. Go to Step 1 and repeat for each microcontroller.

The `generate_table.awk` program reads one of `*.txt` files and prints out an
ASCII table that can be directly embedded into this README.md file. For example
the following command produces the table in the Nano section below:

```
$ ./generate_table.awk < nano.txt
```

Fortunately, we no longer need to run `generate_table.awk` for each `*.txt`
file. The process has been automated using the `generate_readme.py` script which
will be invoked by the following command:
```
$ make README.md
```

The CPU times below are given in microseconds. The "samples" column is the
number of `TimingStats::update()` calls that were made.

## CPU Time Changes

**v0.4**

* Upgrade to ESP8266 v3.0.2 and ESP32 v2.0.2 seem to have made the
  https://github.com/Seeed-Studio/Arduino_Software_I2C library significantly
  slower.
    * The Seeed I2C library does not use any `delayMicroseconds()` between
      successive calls of `digitalWrite()` when transitioning bit levels.
    * This indicates that these GPIO calls got slower.
    * Other I2C libraries are not affected as much because the time consumed
      `delayMicroseconds()` is probably greater than the time consumed by the
      `digitalWrite()` function.

**v0.4.1**

* Rearrange grouping in the order of:
    * AceWire implementations (`SimpleWire*Interface`)
    * `<Wire.h>`
    * third party all platforms
    * third party AVR only

## Results

The following tables show the number of microseconds taken to send 11 bytes to
the DS3231 RTC chip. Each byte sends 8 bits, and reads a response bit from the
slave device, for a total of 9 bits.

The "eff kbps" is the transfer speed in bits per second, which includes the
overhead of the START and STOP conditions.

The following implementations are tested:

* AceWire I2C implementations
    * `SimpleWireInterface,1us`: AceWire's own Software I2C using
      `digitalWrite()` using a `delayMicros` of 1 micros.
    * `SimpleWireFastInterface,1us`: AceWire's own Software I2C using a
      `digitalWriteFast()` library and a `delayMicros` of 1 micros (compatible
      with AVR only).
* Native `<Wire.h>` (all platforms)
    * `TwoWireInterface<TwoWire>,100kHz`: The hardware `<Wire.h>` library set to
      100 kHz (default).
    * `TwoWireInterface<TwoWire>,400kHz`: The hardware `<Wire.h>` library at to
      400 kHz.
* Third party libraries (all platforms)
    * `FeliasFoggWireInterface<SlowSoftWire>`: Software I2C using
      https://github.com/felias-fogg/SlowSoftWire (all platforms)
    * `MarpleWireInterface<SoftWire>`: Software I2C using
      https://github.com/stevemarple/SoftWire (all platforms)
    * `RaemondWireInterface<SoftWire>`: Software I2C using
      https://github.com/RaemondBW/SWire (all platforms)
    * `SeeedWireInterface<SoftwareI2C>`: Software I2C using
      https://github.com/Seeed-Studio/Arduino_Software_I2C (all platforms)
* Third party libraries (AVR only)
    * `TestatoWireInterface<SoftwareWire>,100kHz`: Software I2C using
      https://github.com/Testato/SoftwareWire set to 100 kHz (AVR only).
    * `TestatoWireInterface<SoftwareWire>,400kHz`: Software I2C using
      https://github.com/Testato/SoftwareWire set to 400 kHz (AVR only).

### Arduino Nano

* 16MHz ATmega328P
* Arduino IDE 1.8.19, Arduino CLI 0.19.2
* Arduino AVR Boards 1.8.4
* `micros()` has a resolution of 4 microseconds

```
Sizes of Objects:
sizeof(TwoWireInterface): 2
sizeof(SimpleWireInterface): 5
sizeof(SimpleWireFastInterface<2, 3, 10>): 2

CPU:
+-------------------------------------------+-------------------+----------+
| Functionality                             |   min/  avg/  max | eff kbps |
|-------------------------------------------+-------------------+----------|
| SimpleWireInterface,1us                   |  1644/ 1674/ 1824 |     48.4 |
| SimpleWireFastInterface,1us               |   140/  152/  164 |    532.9 |
|-------------------------------------------+-------------------+----------|
| TwoWireInterface<TwoWire>,100kHz          |   932/  935/  948 |     86.6 |
| TwoWireInterface<TwoWire>,400kHz          |   308/  322/  332 |    251.6 |
|-------------------------------------------+-------------------+----------|
| FeliasFoggWireInterface<SlowSoftWire>     |  1852/ 1870/ 2048 |     43.3 |
| RaemondWireInterface<SoftWire>            |  2504/ 2524/ 2764 |     32.1 |
| SeeedWireInterface<SoftwareI2C>           |  1912/ 1933/ 2116 |     41.9 |
|-------------------------------------------+-------------------+----------|
| TestatoWireInterface<SoftwareWire>,100kHz |  1368/ 1376/ 1480 |     58.9 |
| TestatoWireInterface<SoftwareWire>,400kHz |   988/  996/ 1088 |     81.3 |
+-------------------------------------------+-------------------+----------+

```

### SparkFun Pro Micro

* 16 MHz ATmega32U4
* Arduino IDE 1.8.19, Arduino CLI 0.19.2
* SparkFun AVR Boards 1.1.13
* `micros()` has a resolution of 4 microseconds

```
Sizes of Objects:
sizeof(TwoWireInterface): 2
sizeof(SimpleWireInterface): 5
sizeof(SimpleWireFastInterface<2, 3, 10>): 2

CPU:
+-------------------------------------------+-------------------+----------+
| Functionality                             |   min/  avg/  max | eff kbps |
|-------------------------------------------+-------------------+----------|
| SimpleWireInterface,1us                   |  1648/ 1659/ 1664 |     48.8 |
| SimpleWireFastInterface,1us               |   140/  142/  148 |    570.4 |
|-------------------------------------------+-------------------+----------|
| TwoWireInterface<TwoWire>,100kHz          |   932/  933/  940 |     86.8 |
| TwoWireInterface<TwoWire>,400kHz          |   308/  313/  324 |    258.8 |
|-------------------------------------------+-------------------+----------|
| FeliasFoggWireInterface<SlowSoftWire>     |  1864/ 1868/ 1872 |     43.4 |
| RaemondWireInterface<SoftWire>            |  2688/ 2695/ 2700 |     30.1 |
| SeeedWireInterface<SoftwareI2C>           |  2268/ 2271/ 2284 |     35.7 |
|-------------------------------------------+-------------------+----------|
| TestatoWireInterface<SoftwareWire>,100kHz |  1376/ 1380/ 1388 |     58.7 |
| TestatoWireInterface<SoftwareWire>,400kHz |   984/  992/  996 |     81.7 |
+-------------------------------------------+-------------------+----------+

```

### STM32 Blue Pill

* STM32F103C8, 72 MHz ARM Cortex-M3
* Arduino IDE 1.8.19, Arduino CLI 0.19.2
* STM32duino 2.2.0

```
Sizes of Objects:
sizeof(TwoWireInterface): 4
sizeof(SimpleWireInterface): 5

CPU:
+-------------------------------------------+-------------------+----------+
| Functionality                             |   min/  avg/  max | eff kbps |
|-------------------------------------------+-------------------+----------|
| SimpleWireInterface,1us                   |  1901/ 1905/ 1927 |     42.5 |
|-------------------------------------------+-------------------+----------|
| TwoWireInterface<TwoWire>,100kHz          |   843/  843/  848 |     96.1 |
| TwoWireInterface<TwoWire>,400kHz          |   258/  258/  270 |    314.0 |
|-------------------------------------------+-------------------+----------|
| FeliasFoggWireInterface<SlowSoftWire>     |  1940/ 1946/ 1955 |     41.6 |
| RaemondWireInterface<SoftWire>            |  1956/ 1957/ 1978 |     41.4 |
| SeeedWireInterface<SoftwareI2C>           |   509/  513/  530 |    157.9 |
+-------------------------------------------+-------------------+----------+

```

### ESP8266

* NodeMCU 1.0 clone, 80MHz ESP8266
* Arduino IDE 1.8.19, Arduino CLI 0.19.2
* ESP8266 Boards 3.0.2

```
Sizes of Objects:
sizeof(TwoWireInterface): 4
sizeof(SimpleWireInterface): 5

CPU:
+-------------------------------------------+-------------------+----------+
| Functionality                             |   min/  avg/  max | eff kbps |
|-------------------------------------------+-------------------+----------|
| SimpleWireInterface,1us                   |   816/  819/  867 |     98.9 |
|-------------------------------------------+-------------------+----------|
| TwoWireInterface<TwoWire>,100kHz          |  1105/ 1113/ 1251 |     72.8 |
| TwoWireInterface<TwoWire>,400kHz          |   229/  229/  229 |    353.7 |
|-------------------------------------------+-------------------+----------|
| FeliasFoggWireInterface<SlowSoftWire>     |   828/  831/  895 |     97.5 |
| RaemondWireInterface<SoftWire>            |   988/  992/ 1059 |     81.7 |
| SeeedWireInterface<SoftwareI2C>           |   598/  601/  650 |    134.8 |
+-------------------------------------------+-------------------+----------+

```

### ESP32

* ESP32-01 Dev Board, 240 MHz Tensilica LX6
* Arduino IDE 1.8.19, Arduino CLI 0.19.2
* ESP32 Boards 2.0.2

```
Sizes of Objects:
sizeof(TwoWireInterface): 4
sizeof(SimpleWireInterface): 5

CPU:
+-------------------------------------------+-------------------+----------+
| Functionality                             |   min/  avg/  max | eff kbps |
|-------------------------------------------+-------------------+----------|
| SimpleWireInterface,1us                   |   500/  506/  526 |    160.1 |
|-------------------------------------------+-------------------+----------|
| TwoWireInterface<TwoWire>,100kHz          |   895/  899/  968 |     90.1 |
| TwoWireInterface<TwoWire>,400kHz          |   273/  273/  286 |    296.7 |
|-------------------------------------------+-------------------+----------|
| FeliasFoggWireInterface<SlowSoftWire>     |   515/  521/  540 |    155.5 |
| RaemondWireInterface<SoftWire>            |   403/  409/  430 |    198.0 |
| SeeedWireInterface<SoftwareI2C>           |    74/   75/   87 |   1080.0 |
+-------------------------------------------+-------------------+----------+

```

### Teensy 3.2

* 96 MHz ARM Cortex-M4
* Arduino IDE 1.8.19, Arduino CLI 0.19.2
* Teensyduino 1.56
* Compiler options: "Faster"

```
Sizes of Objects:
sizeof(TwoWireInterface): 4
sizeof(SimpleWireInterface): 5

CPU:
+-------------------------------------------+-------------------+----------+
| Functionality                             |   min/  avg/  max | eff kbps |
|-------------------------------------------+-------------------+----------|
| SimpleWireInterface,1us                   |   367/  368/  374 |    220.1 |
|-------------------------------------------+-------------------+----------|
| TwoWireInterface<TwoWire>,100kHz          |   831/  835/  836 |     97.0 |
| TwoWireInterface<TwoWire>,400kHz          |   229/  229/  230 |    353.7 |
|-------------------------------------------+-------------------+----------|
| FeliasFoggWireInterface<SlowSoftWire>     |   521/  522/  528 |    155.2 |
| RaemondWireInterface<SoftWire>            |   444/  445/  452 |    182.0 |
| SeeedWireInterface<SoftwareI2C>           |   141/  142/  144 |    570.4 |
+-------------------------------------------+-------------------+----------+

```

