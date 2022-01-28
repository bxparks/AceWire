#!/usr/bin/python3
#
# Python script that regenerates the README.md from the embedded template. Uses
# ./generate_table.awk to regenerate the ASCII tables from the various *.txt
# files.

from subprocess import check_output

attiny_results = check_output(
    "./generate_table.awk < attiny.txt", shell=True, text=True)
nano_results = check_output(
    "./generate_table.awk < nano.txt", shell=True, text=True)
micro_results = check_output(
    "./generate_table.awk < micro.txt", shell=True, text=True)
stm32_results = check_output(
    "./generate_table.awk < stm32.txt", shell=True, text=True)
esp8266_results = check_output(
    "./generate_table.awk < esp8266.txt", shell=True, text=True)
esp32_results = check_output(
    "./generate_table.awk < esp32.txt", shell=True, text=True)
teensy32_results = check_output(
    "./generate_table.awk < teensy32.txt", shell=True, text=True)

print(f"""\
# Memory Benchmark

The `MemoryBenchmark.ino` collects the amount of flash and static memory
consumed by different implementations in the AceWire library.

It compiles various code snippets which are controlled by the `FEATURE` macro
flag. The `collect.sh` edits this `FEATURE` flag programmatically, then runs the
Arduino IDE compiler on the program, and extracts the flash and static memory
usage into a text file (e.g. `nano.txt`).

The numbers shown below should be considered to be rough estimates. It is often
difficult to separate out the code size of the library from the overhead imposed
by the runtime environment of the processor. For example, it often seems like
the ESP8266 allocates flash memory in blocks of a certain quantity, so the
calculated flash size can jump around in unexpected ways.

**Version**: AceWire v0.4.1

**DO NOT EDIT**: This file was auto-generated using `make README.md`.

## How to Generate

This requires the [AUniter](https://github.com/bxparks/AUniter) script
to execute the Arduino IDE programmatically.

The `Makefile` has rules for several microcontrollers:

```
$ make benchmarks
```
produces the following files:

```
attiny.txt
nano.txt
micro.txt
stm32.txt
esp8266.txt
esp32.txt
teensy32.txt
```

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

## Library Size Changes

**v0.4**

* Reduce memory consumption for some third party libraries by reusing the
  pre-defined instances of various I2C classes. Analogous to the `Wire` instance
  for the `TwoWire` class.

**v0.4.1**

* Add `TodbotWireInterface` to support https://github.com/todbot/SoftI2CMaster.

## Results

The following shows the flash and static memory sizes of the following 
I2C implementations:

* AceWire I2C implementations
    * `SimpleWireInterface`: AceWire's own Software I2C using `digitalWrite()`.
    * `SimpleWireFastInterface`: AceWire's own Software I2C using a
    `digitalWriteFast()` library. (AVR only)
* Native `<Wire.h>` (all platforms)
    * `TwoWireInterface<TwoWire>`: Hardware I2C using preinstalled `<Wire.h>`.
* Third party libraries (all platforms)
    * `FeliasFoggWireInterface<SlowSoftWire>`: Software I2C using
      https://github.com/felias-fogg/SlowSoftWire
    * `MarpleWireInterface<SoftWire>`: Software I2C using
      https://github.com/stevemarple/SoftWire library.
    * `RaemondWireInterface<SoftWare>`: Software I2C using
      https://github.com/RaemondBW/SWire
    * `SeeedWireInterface<SoftwareI2C>`: Software I2C using
      https://github.com/Seeed-Studio/Arduino_Software_I2C
* Third party libraries (AVR only)
    * `TestatoWireInterface<SoftwareWire>`: Software I2C using
      https://github.com/Testato/SoftwareWire
    * `ThexenoWireInterface<TwoWire>`: Software I2C using
      https://github.com/thexeno/HardWire-Arduino-Library
    * `TodbotWireInterface<SoftI2CMaster>`: Software I2C using
      https://github.com/todbot/SoftI2CMaster

### ATtiny85

* 8MHz ATtiny85
* Arduino IDE 1.8.19, Arduino CLI 0.19.2
* SpenceKonde/ATTinyCore 1.5.2

```
{attiny_results}
```

### Arduino Nano

* 16MHz ATmega328P
* Arduino IDE 1.8.19, Arduino CLI 0.19.2
* Arduino AVR Boards 1.8.4

```
{nano_results}
```

### Sparkfun Pro Micro

* 16 MHz ATmega32U4
* Arduino IDE 1.8.13, Arduino CLI 0.19.2
* SparkFun AVR Boards 1.1.13

```
{micro_results}
```

### STM32 Blue Pill

* STM32F103C8, 72 MHz ARM Cortex-M3
* Arduino IDE 1.8.19, Arduino CLI 0.19.2
* STM32duino 2.2.0

```
{stm32_results}
```

### ESP8266

* NodeMCU 1.0, 80MHz ESP8266
* Arduino IDE 1.8.19, Arduino CLI 0.19.2
* ESP8266 Boards 3.0.2

```
{esp8266_results}
```

### ESP32

* ESP32-01 Dev Board, 240 MHz Tensilica LX6
* Arduino IDE 1.8.19, Arduino CLI 0.19.2
* ESP32 Boards 2.0.2

```
{esp32_results}
```

### Teensy 3.2

* 96 MHz ARM Cortex-M4
* Arduino IDE 1.8.19, Arduino CLI 0.19.2
* Teensyduino 1.56
* Compiler options: "Faster"

```
{teensy32_results}
```
""")
