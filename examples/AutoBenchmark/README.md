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

**v0.2+:**

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
  `digitalWriteFast()` library and a `delayMicros` of 1 micros (compatible with AVR only).
* `TwoWireInterface<SoftwareWire>,100kHz`: Software I2C using
  https://github.com/Testato/SoftwareWire set to 100 kHz (compatible with AVR
  only).
* `TwoWireInterface<SoftwareWire>,400kHz`: Software I2C using
  https://github.com/Testato/SoftwareWire set to 400 kHz (compatible with AVR
  only).
* `TwoWireInterface<SWire>`: Software I2C using
  https://github.com/RaemondBW/SWire (results for ESP32 suspicious, seems
  like `endTransmission()` is terminating prematurely)
* `TwoWireInterface<SlowSoftWire>`: Software I2C using
  https://github.com/felias-fogg/SlowSoftWire
* `TwoWireInterface<SeeedSoftwareI2C>`: Software I2C using
  https://github.com/Seeed-Studio/Arduino_Software_I2C

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
| TwoWireInterface<TwoWire>,100kHz        |  1132/ 1137/ 1160 |     87.1 |
| TwoWireInterface<TwoWire>,400kHz        |   376/  380/  388 |    260.5 |
| SimpleWireInterface,1us                 |  2004/ 2020/ 2208 |     49.0 |
| SimpleWireFastInterface,1us             |   168/  180/  192 |    550.0 |
|-----------------------------------------+-------------------+----------|
| TwoWireInterface<SoftwareWire>,100kHz   |  1664/ 1670/ 1740 |     59.3 |
| TwoWireInterface<SoftwareWire>,400kHz   |  1196/ 1205/ 1324 |     82.2 |
| TwoWireInterface<SWire>                 |  3068/ 3084/ 3380 |     32.1 |
| TwoWireInterface<SlowSoftWire>          |  2260/ 2273/ 2508 |     43.6 |
| TwoWireInterface<SeeedSoftwareI2C>      |  2336/ 2352/ 2576 |     42.1 |
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
| TwoWireInterface<TwoWire>,100kHz        |  1128/ 1134/ 1140 |     87.3 |
| TwoWireInterface<TwoWire>,400kHz        |   376/  377/  388 |    262.6 |
| SimpleWireInterface,1us                 |  2012/ 2016/ 2020 |     49.1 |
| SimpleWireFastInterface,1us             |   168/  171/  176 |    578.9 |
|-----------------------------------------+-------------------+----------|
| TwoWireInterface<SoftwareWire>,100kHz   |  1668/ 1675/ 1680 |     59.1 |
| TwoWireInterface<SoftwareWire>,400kHz   |  1200/ 1205/ 1212 |     82.2 |
| TwoWireInterface<SWire>                 |  3292/ 3296/ 3304 |     30.0 |
| TwoWireInterface<SlowSoftWire>          |  2272/ 2275/ 2284 |     43.5 |
| TwoWireInterface<SeeedSoftwareI2C>      |  2760/ 2768/ 2772 |     35.8 |
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
| TwoWireInterface<TwoWire>,100kHz        |  1041/ 1042/ 1044 |     95.0 |
| TwoWireInterface<TwoWire>,400kHz        |   294/  294/  297 |    336.7 |
| SimpleWireInterface,1us                 |  1650/ 1652/ 1655 |     59.9 |
|-----------------------------------------+-------------------+----------|
| TwoWireInterface<SWire>                 |  1787/ 1789/ 1792 |     55.3 |
| TwoWireInterface<SlowSoftWire>          |  1244/ 1245/ 1253 |     79.5 |
| TwoWireInterface<SeeedSoftwareI2C>      |   811/  814/  816 |    121.6 |
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
| TwoWireInterface<TwoWire>,100kHz        |  1020/ 1021/ 1033 |     97.0 |
| TwoWireInterface<TwoWire>,400kHz        |   305/  305/  308 |    324.6 |
| SimpleWireInterface,1us                 |  2302/ 2304/ 2308 |     43.0 |
|-----------------------------------------+-------------------+----------|
| TwoWireInterface<SWire>                 |  2382/ 2384/ 2394 |     41.5 |
| TwoWireInterface<SlowSoftWire>          |  2368/ 2374/ 2413 |     41.7 |
| TwoWireInterface<SeeedSoftwareI2C>      |   618/  621/  632 |    159.4 |
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
| TwoWireInterface<TwoWire>,100kHz        |  1031/ 1039/ 1189 |     95.3 |
| TwoWireInterface<TwoWire>,400kHz        |   270/  270/  274 |    366.7 |
| SimpleWireInterface,1us                 |  1037/ 1039/ 1077 |     95.3 |
|-----------------------------------------+-------------------+----------|
| TwoWireInterface<SWire>                 |  1130/ 1134/ 1205 |     87.3 |
| TwoWireInterface<SlowSoftWire>          |  1051/ 1057/ 1164 |     93.7 |
| TwoWireInterface<SeeedSoftwareI2C>      |   493/  496/  549 |    199.6 |
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
| TwoWireInterface<TwoWire>,100kHz        |  1082/ 1089/ 1215 |     90.9 |
| TwoWireInterface<TwoWire>,400kHz        |   332/  336/  393 |    294.6 |
| SimpleWireInterface,1us                 |   638/  645/  659 |    153.5 |
|-----------------------------------------+-------------------+----------|
| TwoWireInterface<SWire>                 |   461/  467/  487 |    212.0 |
| TwoWireInterface<SlowSoftWire>          |   633/  642/  664 |    154.2 |
| TwoWireInterface<SeeedSoftwareI2C>      |  1195/ 1198/ 1209 |     82.6 |
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
| TwoWireInterface<TwoWire>,100kHz        |  1014/ 1017/ 1018 |     97.3 |
| TwoWireInterface<TwoWire>,400kHz        |   279/  279/  280 |    354.8 |
| SimpleWireInterface,1us                 |   447/  449/  455 |    220.5 |
|-----------------------------------------+-------------------+----------|
| TwoWireInterface<SWire>                 |   543/  545/  550 |    181.7 |
| TwoWireInterface<SlowSoftWire>          |   661/  663/  668 |    149.3 |
| TwoWireInterface<SeeedSoftwareI2C>      |   173/  173/  175 |    572.3 |
+-----------------------------------------+-------------------+----------+

```

