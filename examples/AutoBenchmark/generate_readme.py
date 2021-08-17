#!/usr/bin/env python3
#
# Python script that regenerates the README.md from the embedded template. Uses
# ./generate_table.awk to regenerate the ASCII tables from the various *.txt
# files.

from subprocess import check_output

nano_results = check_output(
    "./generate_table.awk < nano.txt", shell=True, text=True)
micro_results = check_output(
    "./generate_table.awk < micro.txt", shell=True, text=True)
samd_results = check_output(
    "./generate_table.awk < samd.txt", shell=True, text=True)
stm32_results = check_output(
    "./generate_table.awk < stm32.txt", shell=True, text=True)
esp8266_results = check_output(
    "./generate_table.awk < esp8266.txt", shell=True, text=True)
esp32_results = check_output(
    "./generate_table.awk < esp32.txt", shell=True, text=True)
teensy32_results = check_output(
    "./generate_table.awk < teensy32.txt", shell=True, text=True)

print(f"""\
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
{nano_results}
```

### SparkFun Pro Micro

* 16 MHz ATmega32U4
* Arduino IDE 1.8.13
* SparkFun AVR Boards 1.1.13
* `micros()` has a resolution of 4 microseconds

```
{micro_results}
```

### SAMD21 M0 Mini

* 48 MHz ARM Cortex-M0+
* Arduino IDE 1.8.13
* Sparkfun SAMD Core 1.8.3

```
{samd_results}
```

### STM32

* STM32 "Blue Pill", STM32F103C8, 72 MHz ARM Cortex-M3
* Arduino IDE 1.8.13
* STM32duino 2.0.0

```
{stm32_results}
```

### ESP8266

* NodeMCU 1.0 clone, 80MHz ESP8266
* Arduino IDE 1.8.13
* ESP8266 Boards 2.7.4

```
{esp8266_results}
```

### ESP32

* ESP32-01 Dev Board, 240 MHz Tensilica LX6
* Arduino IDE 1.8.13
* ESP32 Boards 1.0.6

```
{esp32_results}
```

### Teensy 3.2

* 96 MHz ARM Cortex-M4
* Arduino IDE 1.8.13
* Teensyduino 1.53
* Compiler options: "Faster"

```
{teensy32_results}
```
""")
