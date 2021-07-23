# AutoBenchmark

This program determines the speed of various I2C implementations supported by
the AceWire library. To obtain accurate results, an actual I2C device must exist
on the bus. Currently, I use a DS3231 RTC chip because it is relatively small
and inexpensive and I can install one on each of my development boards.

**Version**: AceWire v0.2+

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

**v0.2:**

## Results

The following tables show the number of microseconds taken for sending 11 bytes
to the DS3231 RTC chip. Each byte sends 8 bits, plus an extra response bit from
the slave device is read, for a total of 9 bits.

On AVR processors, the "fast" options are available using one of the
digitalWriteFast libraries whose `digitalWriteFast()` functions can be up to 50X
faster if the `pin` number and `value` parameters are compile-time constants. In
addition, the `digitalWriteFast` functions reduce flash memory consumption by
600-700 bytes for `SoftTmiFastInterface`, `SoftSpiFastInterface`,
`HardSpiFastInterface`, and `SimpleWireFastInterface` compared to their non-fast
equivalents.

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
+-----------------------------------------+-------------------+---------+
| Functionality                           |   min/  avg/  max | eff kHz |
|-----------------------------------------+-------------------+---------|
| TwoWireInterface<TwoWire>,100kHz        |  1128/ 1135/ 1160 |    87.2 |
| TwoWireInterface<TwoWire>,400kHz        |   372/  379/  388 |   261.2 |
| SimpleWireInterface,1us                 |  2004/ 2020/ 2212 |    49.0 |
| SimpleWireFastInterface,1us             |   168/  179/  192 |   553.1 |
| TwoWireInterface<SoftwareWire>,100kHz   |  1668/ 1678/ 1740 |    59.0 |
| TwoWireInterface<SoftwareWire>,400kHz   |  1204/ 1214/ 1336 |    81.5 |
| TwoWireInterface<SWire>                 |  3064/ 3084/ 3388 |    32.1 |
| TwoWireInterface<SlowSoftWire>          |  2256/ 2271/ 2500 |    43.6 |
+-----------------------------------------+-------------------+---------+

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
+-----------------------------------------+-------------------+---------+
| Functionality                           |   min/  avg/  max | eff kHz |
|-----------------------------------------+-------------------+---------|
| TwoWireInterface<TwoWire>,100kHz        |  1124/ 1130/ 1144 |    87.6 |
| TwoWireInterface<TwoWire>,400kHz        |   372/  375/  380 |   264.0 |
| SimpleWireInterface,1us                 |  2012/ 2016/ 2020 |    49.1 |
| SimpleWireFastInterface,1us             |   168/  170/  176 |   582.4 |
| TwoWireInterface<SoftwareWire>,100kHz   |  1676/ 1684/ 1692 |    58.8 |
| TwoWireInterface<SoftwareWire>,400kHz   |  1208/ 1213/ 1224 |    81.6 |
| TwoWireInterface<SWire>                 |  3292/ 3294/ 3304 |    30.1 |
| TwoWireInterface<SlowSoftWire>          |  2268/ 2271/ 2288 |    43.6 |
+-----------------------------------------+-------------------+---------+

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
+-----------------------------------------+-------------------+---------+
| Functionality                           |   min/  avg/  max | eff kHz |
|-----------------------------------------+-------------------+---------|
| TwoWireInterface<TwoWire>,100kHz        |  1041/ 1041/ 1046 |    95.1 |
| TwoWireInterface<TwoWire>,400kHz        |   293/  293/  297 |   337.9 |
| SimpleWireInterface,1us                 |  1647/ 1650/ 1653 |    60.0 |
| TwoWireInterface<SlowSoftWire>          |  1240/ 1242/ 1251 |    79.7 |
+-----------------------------------------+-------------------+---------+

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
+-----------------------------------------+-------------------+---------+
| Functionality                           |   min/  avg/  max | eff kHz |
|-----------------------------------------+-------------------+---------|
| TwoWireInterface<TwoWire>,100kHz        |  1018/ 1019/ 1031 |    97.2 |
| TwoWireInterface<TwoWire>,400kHz        |   303/  304/  307 |   325.7 |
| SimpleWireInterface,1us                 |  2275/ 2277/ 2284 |    43.5 |
| TwoWireInterface<SlowSoftWire>          |  2338/ 2342/ 2350 |    42.3 |
+-----------------------------------------+-------------------+---------+

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
+-----------------------------------------+-------------------+---------+
| Functionality                           |   min/  avg/  max | eff kHz |
|-----------------------------------------+-------------------+---------|
| TwoWireInterface<TwoWire>,100kHz        |  1031/ 1039/ 1204 |    95.3 |
| TwoWireInterface<TwoWire>,400kHz        |   270/  270/  273 |   366.7 |
| SimpleWireInterface,1us                 |  1037/ 1040/ 1095 |    95.2 |
| TwoWireInterface<SlowSoftWire>          |  1051/ 1056/ 1147 |    93.8 |
+-----------------------------------------+-------------------+---------+

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
+-----------------------------------------+-------------------+---------+
| Functionality                           |   min/  avg/  max | eff kHz |
|-----------------------------------------+-------------------+---------|
| TwoWireInterface<TwoWire>,100kHz        |  1080/ 1087/ 1211 |    91.1 |
| TwoWireInterface<TwoWire>,400kHz        |   330/  335/  390 |   295.5 |
| SimpleWireInterface,1us                 |   638/  645/  654 |   153.5 |
| TwoWireInterface<SlowSoftWire>          |   633/  642/  674 |   154.2 |
+-----------------------------------------+-------------------+---------+

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
+-----------------------------------------+-------------------+---------+
| Functionality                           |   min/  avg/  max | eff kHz |
|-----------------------------------------+-------------------+---------|
| TwoWireInterface<TwoWire>,100kHz        |  1015/ 1017/ 1018 |    97.3 |
| TwoWireInterface<TwoWire>,400kHz        |   279/  279/  280 |   354.8 |
| SimpleWireInterface,1us                 |   459/  461/  464 |   214.8 |
| TwoWireInterface<SlowSoftWire>          |   660/  661/  668 |   149.8 |
+-----------------------------------------+-------------------+---------+

```

