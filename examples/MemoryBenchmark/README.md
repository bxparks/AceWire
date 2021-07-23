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

**Version**: AceWire v0.2+

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
samd.txt
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

**v0.2+**

* Initial iteration of MemoryBenchmarks.

## Results

The following shows the flash and static memory sizes of the following 
I2C implementations:

* `TwoWireInterface<TwoWire>`: The hardware `<Wire.h>` library.
* `SimpleWireInterface`: Software I2C using `digitalWrite()`.
* `SimpleWireFastInterface`: Software I2C using a `digitalWriteFast()` library.
  (AVR only.)
* `TwoWireInterface<SoftwareWire>`: Software I2C using
  https://github.com/Testato/SoftwareWire. (AVR only.)
* `TwoWireInterface<SWire>`: Software I2C using
  https://github.com/RaemondBW/SWire
* `TwoWireInterface<SlowSoftWire>`: Software I2C using
  https://github.com/felias-fogg/SlowSoftWire

### ATtiny85

* 8MHz ATtiny85
* Arduino IDE 1.8.13
* SpenceKonde/ATTinyCore 1.5.2

```
+--------------------------------------------------------------+
| functionality                   |  flash/  ram |       delta |
|---------------------------------+--------------+-------------|
| baseline                        |    260/   11 |     0/    0 |
|---------------------------------+--------------+-------------|
| TwoWireInterface<TwoWire>       |   1014/   55 |   754/   44 |
| SimpleWireInterface             |    994/   16 |   734/    5 |
| SimpleWireFastInterface         |    448/   13 |   188/    2 |
| TwoWireInterface<SoftwareWire>  |   1962/   72 |  1702/   61 |
| TwoWireInterface<SWire>         |   1354/  157 |  1094/  146 |
| TwoWireInterface<SlowSoftWire>  |   1570/   81 |  1310/   70 |
+--------------------------------------------------------------+

```

### Arduino Nano

* 16MHz ATmega328P
* Arduino IDE 1.8.13
* Arduino AVR Boards 1.8.3

```
+--------------------------------------------------------------+
| functionality                   |  flash/  ram |       delta |
|---------------------------------+--------------+-------------|
| baseline                        |    456/   11 |     0/    0 |
|---------------------------------+--------------+-------------|
| TwoWireInterface<TwoWire>       |   2914/  229 |  2458/  218 |
| SimpleWireInterface             |   1306/   16 |   850/    5 |
| SimpleWireFastInterface         |    714/   13 |   258/    2 |
| TwoWireInterface<SoftwareWire>  |   2442/   72 |  1986/   61 |
| TwoWireInterface<SWire>         |   1686/  157 |  1230/  146 |
| TwoWireInterface<SlowSoftWire>  |   1912/   83 |  1456/   72 |
+--------------------------------------------------------------+

```

### Sparkfun Pro Micro

* 16 MHz ATmega32U4
* Arduino IDE 1.8.13
* SparkFun AVR Boards 1.1.13

```
+--------------------------------------------------------------+
| functionality                   |  flash/  ram |       delta |
|---------------------------------+--------------+-------------|
| baseline                        |   3472/  151 |     0/    0 |
|---------------------------------+--------------+-------------|
| TwoWireInterface<TwoWire>       |   5898/  369 |  2426/  218 |
| SimpleWireInterface             |   4398/  156 |   926/    5 |
| SimpleWireFastInterface         |   3730/  153 |   258/    2 |
| TwoWireInterface<SoftwareWire>  |   5376/  212 |  1904/   61 |
| TwoWireInterface<SWire>         |   4778/  297 |  1306/  146 |
| TwoWireInterface<SlowSoftWire>  |   4974/  223 |  1502/   72 |
+--------------------------------------------------------------+

```

### SAMD21 M0 Mini

* 48 MHz ARM Cortex-M0+
* Arduino IDE 1.8.13
* Sparkfun SAMD Core 1.8.3

```
+--------------------------------------------------------------+
| functionality                   |  flash/  ram |       delta |
|---------------------------------+--------------+-------------|
| baseline                        |   9940/    0 |     0/    0 |
|---------------------------------+--------------+-------------|
| TwoWireInterface<TwoWire>       |  11452/    0 |  1512/    0 |
| SimpleWireInterface             |  10440/    0 |   500/    0 |
| TwoWireInterface<SWire>         |  10868/    0 |   928/    0 |
| TwoWireInterface<SlowSoftWire>  |  11000/    0 |  1060/    0 |
+--------------------------------------------------------------+

```

### STM32 Blue Pill

* STM32F103C8, 72 MHz ARM Cortex-M3
* Arduino IDE 1.8.13
* STM32duino 2.0.0

```
+--------------------------------------------------------------+
| functionality                   |  flash/  ram |       delta |
|---------------------------------+--------------+-------------|
| baseline                        |  21420/ 3536 |     0/    0 |
|---------------------------------+--------------+-------------|
| TwoWireInterface<TwoWire>       |  28872/ 3740 |  7452/  204 |
| SimpleWireInterface             |  24140/ 3560 |  2720/   24 |
| TwoWireInterface<SWire>         |  24560/ 3700 |  3140/  164 |
| TwoWireInterface<SlowSoftWire>  |  24940/ 3612 |  3520/   76 |
+--------------------------------------------------------------+

```

### ESP8266

* NodeMCU 1.0, 80MHz ESP8266
* Arduino IDE 1.8.13
* ESP8266 Boards 2.7.4

```
+--------------------------------------------------------------+
| functionality                   |  flash/  ram |       delta |
|---------------------------------+--------------+-------------|
| baseline                        | 256700/26784 |     0/    0 |
|---------------------------------+--------------+-------------|
| TwoWireInterface<TwoWire>       | 261372/27268 |  4672/  484 |
| SimpleWireInterface             | 257732/26796 |  1032/   12 |
| TwoWireInterface<SWire>         | 258376/26944 |  1676/  160 |
| TwoWireInterface<SlowSoftWire>  | 259828/26876 |  3128/   92 |
+--------------------------------------------------------------+

```

### ESP32

* ESP32-01 Dev Board, 240 MHz Tensilica LX6
* Arduino IDE 1.8.13
* ESP32 Boards 1.0.6

```
+--------------------------------------------------------------+
| functionality                   |  flash/  ram |       delta |
|---------------------------------+--------------+-------------|
| baseline                        | 197748/13084 |     0/    0 |
|---------------------------------+--------------+-------------|
| TwoWireInterface<TwoWire>       | 208734/13992 | 10986/  908 |
| SimpleWireInterface             | 199350/13256 |  1602/  172 |
| TwoWireInterface<SWire>         | 200270/13408 |  2522/  324 |
| TwoWireInterface<SlowSoftWire>  | 201474/13336 |  3726/  252 |
+--------------------------------------------------------------+

```

### Teensy 3.2

* 96 MHz ARM Cortex-M4
* Arduino IDE 1.8.13
* Teensyduino 1.53
* Compiler options: "Faster"

```
+--------------------------------------------------------------+
| functionality                   |  flash/  ram |       delta |
|---------------------------------+--------------+-------------|
| baseline                        |  10880/ 4152 |     0/    0 |
|---------------------------------+--------------+-------------|
| TwoWireInterface<TwoWire>       |  14764/ 4824 |  3884/  672 |
| SimpleWireInterface             |  12332/ 4160 |  1452/    8 |
| TwoWireInterface<SWire>         |  12492/ 4300 |  1612/  148 |
| TwoWireInterface<SlowSoftWire>  |  12476/ 4212 |  1596/   60 |
+--------------------------------------------------------------+

```

