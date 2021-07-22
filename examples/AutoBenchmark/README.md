# AutoBenchmark

This program determines the speed of various I2C implementations supported by
the AceWire library. To obtain accurate results, an actual I2C device must exist
on the bus. In this particular example, an HT16K33 LED module was used because
AceWire was developed in coordination with AceSegment where the HT16K33 was
supported.

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

The following tables show the number of microseconds taken for sending 12 bytes
to the HT16K33 LED module. Each byte sends 8 bits, plus an extra response bit
from the slave device is read, for a total of 9 bits.

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
| TwoWireInterface<TwoWire>,100kHz        |  1236/ 1242/ 1268 |    87.0 |
| TwoWireInterface<TwoWire>,400kHz        |   408/  417/  444 |   259.0 |
| SimpleWireInterface,1us                 |  2144/ 2159/ 2360 |    50.0 |
| SimpleWireFastInterface,1us             |   276/  290/  316 |   372.4 |
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
| TwoWireInterface<TwoWire>,100kHz        |  1232/ 1236/ 1240 |    87.4 |
| TwoWireInterface<TwoWire>,400kHz        |   408/  412/  416 |   262.1 |
| SimpleWireInterface,1us                 |  2156/ 2161/ 2172 |    50.0 |
| SimpleWireFastInterface,1us             |   276/  281/  292 |   384.3 |
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
| TwoWireInterface<TwoWire>,100kHz        |  1136/ 1136/ 1138 |    95.1 |
| TwoWireInterface<TwoWire>,400kHz        |   320/  320/  324 |   337.5 |
| SimpleWireInterface,1us                 |  1794/ 1798/ 1800 |    60.1 |
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
| TwoWireInterface<TwoWire>,100kHz        |  1113/ 1113/ 1114 |    97.0 |
| TwoWireInterface<TwoWire>,400kHz        |   333/  333/  334 |   324.3 |
| SimpleWireInterface,1us                 |  2569/ 2572/ 2575 |    42.0 |
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
| TwoWireInterface<TwoWire>,100kHz        |  1124/ 1124/ 1136 |    96.1 |
| TwoWireInterface<TwoWire>,400kHz        |   295/  295/  296 |   366.1 |
| SimpleWireInterface,1us                 |  1131/ 1132/ 1146 |    95.4 |
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
| TwoWireInterface<TwoWire>,100kHz        |  1174/ 1175/ 1183 |    91.9 |
| TwoWireInterface<TwoWire>,400kHz        |   357/  357/  362 |   302.5 |
| SimpleWireInterface,1us                 |   697/  702/  709 |   153.8 |
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
| TwoWireInterface<TwoWire>,100kHz        |  1108/ 1108/ 1113 |    97.5 |
| TwoWireInterface<TwoWire>,400kHz        |   304/  304/  306 |   355.3 |
| SimpleWireInterface,1us                 |   495/  496/  502 |   217.7 |
+-----------------------------------------+-------------------+---------+

```

