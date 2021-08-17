# AutoBenchmark

This program determines the speed of various I2C implementations supported by
the AceWire library. To obtain accurate results, an actual I2C device must exist
on the bus. Currently, I use a DS3231 RTC chip because it is relatively small
and inexpensive and I can install one on each of my development boards.

**Version**: AceWire v0.3.2

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

**v0.3**

## Results

The following tables show the number of microseconds taken to send 11 bytes to
the DS3231 RTC chip. Each byte sends 8 bits, and reads a response bit from the
slave device, for a total of 9 bits.

The "eff kbps" is the transfer speed in bits per second, which includes the
overhead of the START and STOP conditions.

The following implementations are tested:

* `TwoWireInterface<TwoWire>,100kHz`: The hardware `<Wire.h>` library set to 100
  kHz (default).
* `TwoWireInterface<TwoWire>,400kHz`: The hardware `<Wire.h>` library at to 400
  kHz (default).
* `SimpleWireInterface,1us`: AceWire's own Software I2C using `digitalWrite()`
  using a `delayMicros` of 1 micros.
* `SimpleWireFastInterface,1us`: AceWire's own Software I2C using a
  `digitalWriteFast()` library and a `delayMicros` of 1 micros (compatible with
  AVR only).
* Third party libraries
    * `TwoWireInterface<SoftwareWire>,100kHz`: Software I2C using
      https://github.com/Testato/SoftwareWire set to 100 kHz (compatible with
      AVR only).
    * `TwoWireInterface<SoftwareWire>,400kHz`: Software I2C using
      https://github.com/Testato/SoftwareWire set to 400 kHz (compatible with
      AVR only).
    * `TwoWireInterface<SWire>`: Software I2C using
      https://github.com/RaemondBW/SWire
    * `TwoWireInterface<SlowSoftWire>`: Software I2C using
      https://github.com/felias-fogg/SlowSoftWire
    * `TwoWireInterface<SeeedSoftwareI2C>`: Software I2C using
      https://github.com/Seeed-Studio/Arduino_Software_I2C
        * [Seeed SoftwareI2C](https://github.com/Seeed-Studio/Arduino_Software_I2C)
          does not insert any `delayMicroseconds()` between transitions of SDA
          and SCL signals.
        * On sufficiently fast 32-bit processors (e.g. ESP32 and Teensy32), the
          clock rate of the signals becomes faster than the I2C specifications
          and the slave device will not work.

### Arduino Nano

* 16MHz ATmega328P
* Arduino IDE 1.8.13
* Arduino AVR Boards 1.8.3
* `micros()` has a resolution of 4 microseconds

```
Sizes of Objects:
sizeof(TwoWireInterface): 2
sizeof(SimpleWireInterface): 5
sizeof(SimpleWireFastInterface<2, 3, 10>): 2

CPU:
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

### SparkFun Pro Micro

* 16 MHz ATmega32U4
* Arduino IDE 1.8.13
* SparkFun AVR Boards 1.1.13
* `micros()` has a resolution of 4 microseconds

```
Sizes of Objects:
sizeof(TwoWireInterface): 2
sizeof(SimpleWireInterface): 5
sizeof(SimpleWireFastInterface<2, 3, 10>): 2

CPU:
+-----------------------------------------+-------------------+----------+
| Functionality                           |   min/  avg/  max | eff kbps |
|-----------------------------------------+-------------------+----------|
| TwoWireInterface<TwoWire>,100kHz        |   932/  934/  944 |     86.7 |
| TwoWireInterface<TwoWire>,400kHz        |   308/  313/  320 |    258.8 |
| SimpleWireInterface,1us                 |  1652/ 1658/ 1664 |     48.9 |
| SimpleWireFastInterface,1us             |   140/  142/  148 |    570.4 |
|-----------------------------------------+-------------------+----------|
| TwoWireInterface<SoftwareWire>,100kHz   |  1372/ 1378/ 1388 |     58.8 |
| TwoWireInterface<SoftwareWire>,400kHz   |   992/  992/  996 |     81.7 |
| TwoWireInterface<SWire>                 |  2688/ 2695/ 2700 |     30.1 |
| TwoWireInterface<SlowSoftWire>          |  1860/ 1867/ 1872 |     43.4 |
| TwoWireInterface<SeeedSoftwareI2C>      |  1812/ 1821/ 1824 |     44.5 |
+-----------------------------------------+-------------------+----------+

```

### SAMD21 M0 Mini

* 48 MHz ARM Cortex-M0+
* Arduino IDE 1.8.13
* Sparkfun SAMD Core 1.8.3

```
Sizes of Objects:
sizeof(TwoWireInterface): 4
sizeof(SimpleWireInterface): 5

CPU:
+-----------------------------------------+-------------------+----------+
| Functionality                           |   min/  avg/  max | eff kbps |
|-----------------------------------------+-------------------+----------|
| TwoWireInterface<TwoWire>,100kHz        |   853/  853/  854 |     95.0 |
| TwoWireInterface<TwoWire>,400kHz        |   241/  241/  243 |    336.1 |
| SimpleWireInterface,1us                 |  1356/ 1359/ 1363 |     59.6 |
|-----------------------------------------+-------------------+----------|
| TwoWireInterface<SWire>                 |  1451/ 1454/ 1458 |     55.7 |
| TwoWireInterface<SlowSoftWire>          |  1024/ 1024/ 1027 |     79.1 |
| TwoWireInterface<SeeedSoftwareI2C>      |   520/  523/  526 |    154.9 |
+-----------------------------------------+-------------------+----------+

```

### STM32

* STM32 "Blue Pill", STM32F103C8, 72 MHz ARM Cortex-M3
* Arduino IDE 1.8.13
* STM32duino 2.0.0

```
Sizes of Objects:
sizeof(TwoWireInterface): 4
sizeof(SimpleWireInterface): 5

CPU:
+-----------------------------------------+-------------------+----------+
| Functionality                           |   min/  avg/  max | eff kbps |
|-----------------------------------------+-------------------+----------|
| TwoWireInterface<TwoWire>,100kHz        |   841/  841/  849 |     96.3 |
| TwoWireInterface<TwoWire>,400kHz        |   255/  256/  261 |    316.4 |
| SimpleWireInterface,1us                 |  1937/ 1941/ 1942 |     41.7 |
|-----------------------------------------+-------------------+----------|
| TwoWireInterface<SWire>                 |  1988/ 1989/ 1990 |     40.7 |
| TwoWireInterface<SlowSoftWire>          |  1998/ 2000/ 2012 |     40.5 |
| TwoWireInterface<SeeedSoftwareI2C>      |   398/  400/  408 |    202.5 |
+-----------------------------------------+-------------------+----------+

```

### ESP8266

* NodeMCU 1.0 clone, 80MHz ESP8266
* Arduino IDE 1.8.13
* ESP8266 Boards 2.7.4

```
Sizes of Objects:
sizeof(TwoWireInterface): 4
sizeof(SimpleWireInterface): 5

CPU:
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

### ESP32

* ESP32-01 Dev Board, 240 MHz Tensilica LX6
* Arduino IDE 1.8.13
* ESP32 Boards 1.0.6

```
Sizes of Objects:
sizeof(TwoWireInterface): 4
sizeof(SimpleWireInterface): 5

CPU:
+-----------------------------------------+-------------------+----------+
| Functionality                           |   min/  avg/  max | eff kbps |
|-----------------------------------------+-------------------+----------|
| TwoWireInterface<TwoWire>,100kHz        |   899/  906/ 1032 |     89.4 |
| TwoWireInterface<TwoWire>,400kHz        |   284/  287/  358 |    282.2 |
| SimpleWireInterface,1us                 |   524/  530/  539 |    152.8 |
|-----------------------------------------+-------------------+----------|
| TwoWireInterface<SWire>                 |   424/  433/  454 |    187.1 |
| TwoWireInterface<SlowSoftWire>          |   520/  530/  554 |    152.8 |
| TwoWireInterface<SeeedSoftwareI2C>      |    58/   59/   70 |   1372.9 |
+-----------------------------------------+-------------------+----------+

```

### Teensy 3.2

* 96 MHz ARM Cortex-M4
* Arduino IDE 1.8.13
* Teensyduino 1.53
* Compiler options: "Faster"

```
Sizes of Objects:
sizeof(TwoWireInterface): 4
sizeof(SimpleWireInterface): 5

CPU:
+-----------------------------------------+-------------------+----------+
| Functionality                           |   min/  avg/  max | eff kbps |
|-----------------------------------------+-------------------+----------|
| TwoWireInterface<TwoWire>,100kHz        |   832/  836/  837 |     96.9 |
| TwoWireInterface<TwoWire>,400kHz        |   229/  229/  230 |    353.7 |
| SimpleWireInterface,1us                 |   389/  390/  396 |    207.7 |
|-----------------------------------------+-------------------+----------|
| TwoWireInterface<SWire>                 |   457/  457/  463 |    177.2 |
| TwoWireInterface<SlowSoftWire>          |   552/  553/  559 |    146.5 |
| TwoWireInterface<SeeedSoftwareI2C>      |   119/  119/  121 |    680.7 |
+-----------------------------------------+-------------------+----------+

```

