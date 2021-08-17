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

**Version**: AceWire v0.3.2

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

**v0.3*

* Initial iteration of MemoryBenchmarks.

## Results

The following shows the flash and static memory sizes of the following 
I2C implementations:

* `TwoWireInterface<TwoWire>`: The hardware `<Wire.h>` library.
* `SimpleWireInterface`: AceWire's own Software I2C using `digitalWrite()`.
* `SimpleWireFastInterface`: AceWire's own Software I2C using a
  `digitalWriteFast()` library. (AVR only)
* Third party libraries
    * `TwoWireInterface<SoftwareWire>`: Software I2C using
    https://github.com/Testato/SoftwareWire. (AVR only)
    * `TwoWireInterface<SWire>`: Software I2C using
    https://github.com/RaemondBW/SWire
    * `TwoWireInterface<SlowSoftWire>`: Software I2C using
    https://github.com/felias-fogg/SlowSoftWire
    * `TwoWireInterface<SeeedSoftwareI2C>`: Software I2C using
    https://github.com/Seeed-Studio/Arduino_Software_I2C

### ATtiny85

* 8MHz ATtiny85
* Arduino IDE 1.8.13
* SpenceKonde/ATTinyCore 1.5.2

```
+------------------------------------------------------------------+
| functionality                       |  flash/  ram |       delta |
|-------------------------------------+--------------+-------------|
| baseline                            |    260/   11 |     0/    0 |
|-------------------------------------+--------------+-------------|
| TwoWireInterface<TwoWire>           |   1026/   55 |   766/   44 |
| SimpleWireInterface                 |   1020/   16 |   760/    5 |
| SimpleWireFastInterface             |    444/   13 |   184/    2 |
|-------------------------------------+--------------+-------------|
| TwoWireInterface<SoftwareWire>      |   1974/   72 |  1714/   61 |
| TwoWireInterface<SWire>             |   1366/  157 |  1106/  146 |
| TwoWireInterface<SlowSoftWire>      |   1660/   81 |  1400/   70 |
| TwoWireInterface<SeeedSoftwareI2C>  |   1308/   21 |  1048/   10 |
+------------------------------------------------------------------+

```

### Arduino Nano

* 16MHz ATmega328P
* Arduino IDE 1.8.13
* Arduino AVR Boards 1.8.3

```
+------------------------------------------------------------------+
| functionality                       |  flash/  ram |       delta |
|-------------------------------------+--------------+-------------|
| baseline                            |    456/   11 |     0/    0 |
|-------------------------------------+--------------+-------------|
| TwoWireInterface<TwoWire>           |   2926/  229 |  2470/  218 |
| SimpleWireInterface                 |   1336/   16 |   880/    5 |
| SimpleWireFastInterface             |    714/   13 |   258/    2 |
|-------------------------------------+--------------+-------------|
| TwoWireInterface<SoftwareWire>      |   2454/   72 |  1998/   61 |
| TwoWireInterface<SWire>             |   1698/  157 |  1242/  146 |
| TwoWireInterface<SlowSoftWire>      |   2008/   83 |  1552/   72 |
| TwoWireInterface<SeeedSoftwareI2C>  |   1614/   21 |  1158/   10 |
+------------------------------------------------------------------+

```

### Sparkfun Pro Micro

* 16 MHz ATmega32U4
* Arduino IDE 1.8.13
* SparkFun AVR Boards 1.1.13

```
+------------------------------------------------------------------+
| functionality                       |  flash/  ram |       delta |
|-------------------------------------+--------------+-------------|
| baseline                            |   3472/  151 |     0/    0 |
|-------------------------------------+--------------+-------------|
| TwoWireInterface<TwoWire>           |   5910/  369 |  2438/  218 |
| SimpleWireInterface                 |   4428/  156 |   956/    5 |
| SimpleWireFastInterface             |   3728/  153 |   256/    2 |
|-------------------------------------+--------------+-------------|
| TwoWireInterface<SoftwareWire>      |   5388/  212 |  1916/   61 |
| TwoWireInterface<SWire>             |   4790/  297 |  1318/  146 |
| TwoWireInterface<SlowSoftWire>      |   5070/  223 |  1598/   72 |
| TwoWireInterface<SeeedSoftwareI2C>  |   4632/  161 |  1160/   10 |
+------------------------------------------------------------------+

```

### SAMD21 M0 Mini

* 48 MHz ARM Cortex-M0+
* Arduino IDE 1.8.13
* Sparkfun SAMD Core 1.8.3

```
+------------------------------------------------------------------+
| functionality                       |  flash/  ram |       delta |
|-------------------------------------+--------------+-------------|
| baseline                            |   9940/    0 |     0/    0 |
|-------------------------------------+--------------+-------------|
| TwoWireInterface<TwoWire>           |  11468/    0 |  1528/    0 |
| SimpleWireInterface                 |  10448/    0 |   508/    0 |
|-------------------------------------+--------------+-------------|
| TwoWireInterface<SWire>             |  10888/    0 |   948/    0 |
| TwoWireInterface<SlowSoftWire>      |  11084/    0 |  1144/    0 |
| TwoWireInterface<SeeedSoftwareI2C>  |  10600/    0 |   660/    0 |
+------------------------------------------------------------------+

```

### STM32 Blue Pill

* STM32F103C8, 72 MHz ARM Cortex-M3
* Arduino IDE 1.8.13
* STM32duino 2.0.0

```
+------------------------------------------------------------------+
| functionality                       |  flash/  ram |       delta |
|-------------------------------------+--------------+-------------|
| baseline                            |  21420/ 3536 |     0/    0 |
|-------------------------------------+--------------+-------------|
| TwoWireInterface<TwoWire>           |  28888/ 3744 |  7468/  208 |
| SimpleWireInterface                 |  24148/ 3564 |  2728/   28 |
|-------------------------------------+--------------+-------------|
| TwoWireInterface<SWire>             |  24580/ 3704 |  3160/  168 |
| TwoWireInterface<SlowSoftWire>      |  25016/ 3616 |  3596/   80 |
| TwoWireInterface<SeeedSoftwareI2C>  |  24244/ 3576 |  2824/   40 |
+------------------------------------------------------------------+

```

### ESP8266

* NodeMCU 1.0, 80MHz ESP8266
* Arduino IDE 1.8.13
* ESP8266 Boards 2.7.4

```
+------------------------------------------------------------------+
| functionality                       |  flash/  ram |       delta |
|-------------------------------------+--------------+-------------|
| baseline                            | 256700/26784 |     0/    0 |
|-------------------------------------+--------------+-------------|
| TwoWireInterface<TwoWire>           | 261404/27268 |  4704/  484 |
| SimpleWireInterface                 | 257780/26804 |  1080/   20 |
|-------------------------------------+--------------+-------------|
| TwoWireInterface<SWire>             | 258408/26944 |  1708/  160 |
| TwoWireInterface<SlowSoftWire>      | 259972/26884 |  3272/  100 |
| TwoWireInterface<SeeedSoftwareI2C>  | 257904/26812 |  1204/   28 |
+------------------------------------------------------------------+

```

### ESP32

* ESP32-01 Dev Board, 240 MHz Tensilica LX6
* Arduino IDE 1.8.13
* ESP32 Boards 1.0.6

```
+------------------------------------------------------------------+
| functionality                       |  flash/  ram |       delta |
|-------------------------------------+--------------+-------------|
| baseline                            | 197748/13084 |     0/    0 |
|-------------------------------------+--------------+-------------|
| TwoWireInterface<TwoWire>           | 208766/13992 | 11018/  908 |
| SimpleWireInterface                 | 199338/13264 |  1590/  180 |
|-------------------------------------+--------------+-------------|
| TwoWireInterface<SWire>             | 200298/13408 |  2550/  324 |
| TwoWireInterface<SlowSoftWire>      | 201634/13336 |  3886/  252 |
| TwoWireInterface<SeeedSoftwareI2C>  | 200094/13288 |  2346/  204 |
+------------------------------------------------------------------+

```

### Teensy 3.2

* 96 MHz ARM Cortex-M4
* Arduino IDE 1.8.13
* Teensyduino 1.53
* Compiler options: "Faster"

```
+------------------------------------------------------------------+
| functionality                       |  flash/  ram |       delta |
|-------------------------------------+--------------+-------------|
| baseline                            |  10880/ 4152 |     0/    0 |
|-------------------------------------+--------------+-------------|
| TwoWireInterface<TwoWire>           |  14776/ 4824 |  3896/  672 |
| SimpleWireInterface                 |  12400/ 4160 |  1520/    8 |
|-------------------------------------+--------------+-------------|
| TwoWireInterface<SWire>             |  12512/ 4300 |  1632/  148 |
| TwoWireInterface<SlowSoftWire>      |  12584/ 4212 |  1704/   60 |
| TwoWireInterface<SeeedSoftwareI2C>  |  12388/ 4172 |  1508/   20 |
+------------------------------------------------------------------+

```

