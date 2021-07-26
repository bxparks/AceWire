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
| TwoWireInterface<TwoWire>,100kHz        |  1128/ 1133/ 1140 |     87.4 |
| TwoWireInterface<TwoWire>,400kHz        |   376/  379/  384 |    261.2 |
| SimpleWireInterface,1us                 |  2016/ 2019/ 2024 |     49.0 |
| SimpleWireFastInterface,1us             |   168/  172/  176 |    575.6 |
|-----------------------------------------+-------------------+----------|
| TwoWireInterface<SoftwareWire>,100kHz   |  1668/ 1676/ 1680 |     59.1 |
| TwoWireInterface<SoftwareWire>,400kHz   |  1200/ 1205/ 1216 |     82.2 |
| TwoWireInterface<SWire>                 |  3292/ 3297/ 3304 |     30.0 |
| TwoWireInterface<SlowSoftWire>          |  2272/ 2275/ 2284 |     43.5 |
| TwoWireInterface<SeeedSoftwareI2C>      |  2212/ 2217/ 2224 |     44.7 |
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
| TwoWireInterface<TwoWire>,100kHz        |  1041/ 1042/ 1046 |     95.0 |
| TwoWireInterface<TwoWire>,400kHz        |   293/  293/  297 |    337.9 |
| SimpleWireInterface,1us                 |  1650/ 1652/ 1655 |     59.9 |
|-----------------------------------------+-------------------+----------|
| TwoWireInterface<SWire>                 |  1770/ 1772/ 1775 |     55.9 |
| TwoWireInterface<SlowSoftWire>          |  1243/ 1244/ 1248 |     79.6 |
| TwoWireInterface<SeeedSoftwareI2C>      |   633/  636/  640 |    155.7 |
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
| TwoWireInterface<TwoWire>,100kHz        |  1023/ 1024/ 1028 |     96.7 |
| TwoWireInterface<TwoWire>,400kHz        |   307/  308/  312 |    321.4 |
| SimpleWireInterface,1us                 |  2356/ 2358/ 2362 |     42.0 |
|-----------------------------------------+-------------------+----------|
| TwoWireInterface<SWire>                 |  2420/ 2423/ 2446 |     40.9 |
| TwoWireInterface<SlowSoftWire>          |  2429/ 2433/ 2446 |     40.7 |
| TwoWireInterface<SeeedSoftwareI2C>      |   486/  488/  494 |    202.9 |
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
| TwoWireInterface<TwoWire>,100kHz        |  1031/ 1040/ 1205 |     95.2 |
| TwoWireInterface<TwoWire>,400kHz        |   270/  270/  274 |    366.7 |
| SimpleWireInterface,1us                 |  1037/ 1040/ 1096 |     95.2 |
|-----------------------------------------+-------------------+----------|
| TwoWireInterface<SWire>                 |  1130/ 1134/ 1206 |     87.3 |
| TwoWireInterface<SlowSoftWire>          |  1051/ 1056/ 1143 |     93.8 |
| TwoWireInterface<SeeedSoftwareI2C>      |   383/  386/  444 |    256.5 |
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
| TwoWireInterface<TwoWire>,100kHz        |  1082/ 1090/ 1219 |     90.8 |
| TwoWireInterface<TwoWire>,400kHz        |   332/  336/  407 |    294.6 |
| SimpleWireInterface,1us                 |   637/  645/  659 |    153.5 |
|-----------------------------------------+-------------------+----------|
| TwoWireInterface<SWire>                 |   518/  527/  548 |    187.9 |
| TwoWireInterface<SlowSoftWire>          |   631/  642/  667 |    154.2 |
| TwoWireInterface<SeeedSoftwareI2C>      |    71/  123/  878 |    804.9 |
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
| TwoWireInterface<TwoWire>,100kHz        |  1013/ 1017/ 1019 |     97.3 |
| TwoWireInterface<TwoWire>,400kHz        |   279/  279/  282 |    354.8 |
| SimpleWireInterface,1us                 |   453/  455/  461 |    217.6 |
|-----------------------------------------+-------------------+----------|
| TwoWireInterface<SWire>                 |   550/  551/  557 |    179.7 |
| TwoWireInterface<SlowSoftWire>          |   650/  652/  657 |    151.8 |
| TwoWireInterface<SeeedSoftwareI2C>      |   137/  138/  140 |    717.4 |
+-----------------------------------------+-------------------+----------+

```

