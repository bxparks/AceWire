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
+--------------------------------------------------------------------+
| functionality                         |  flash/  ram |       delta |
|---------------------------------------+--------------+-------------|
| baseline                              |    260/   11 |     0/    0 |
|---------------------------------------+--------------+-------------|
| SimpleWireInterface                   |   1020/   16 |   760/    5 |
| SimpleWireFastInterface               |    444/   13 |   184/    2 |
|---------------------------------------+--------------+-------------|
| TwoWireInterface<TwoWire>             |   1026/   55 |   766/   44 |
|---------------------------------------+--------------+-------------|
| FeliasFoggWireInterface<SlowSoftWire> |   1660/   81 |  1400/   70 |
| MarpleWireInterface<SoftWire>         |   2610/  133 |  2350/  122 |
| RaemondWireInterface<SWire>           |   1342/   85 |  1082/   74 |
| SeeedWireInterface<SoftwareI2C>       |   1318/   21 |  1058/   10 |
|---------------------------------------+--------------+-------------|
| TestatoWireInterface<SoftwareWire>    |   2230/   72 |  1970/   61 |
+--------------------------------------------------------------------+

```

### Arduino Nano

* 16MHz ATmega328P
* Arduino IDE 1.8.19, Arduino CLI 0.19.2
* Arduino AVR Boards 1.8.4

```
+--------------------------------------------------------------------+
| functionality                         |  flash/  ram |       delta |
|---------------------------------------+--------------+-------------|
| baseline                              |    456/   11 |     0/    0 |
|---------------------------------------+--------------+-------------|
| SimpleWireInterface                   |   1336/   16 |   880/    5 |
| SimpleWireFastInterface               |    714/   13 |   258/    2 |
|---------------------------------------+--------------+-------------|
| TwoWireInterface<TwoWire>             |   2926/  229 |  2470/  218 |
|---------------------------------------+--------------+-------------|
| FeliasFoggWireInterface<SlowSoftWire> |   2008/   83 |  1552/   72 |
| MarpleWireInterface<SoftWire>         |   2936/  135 |  2480/  124 |
| RaemondWireInterface<SWire>           |   1674/   85 |  1218/   74 |
| SeeedWireInterface<SoftwareI2C>       |   1626/   21 |  1170/   10 |
|---------------------------------------+--------------+-------------|
| TestatoWireInterface<SoftwareWire>    |   2454/   72 |  1998/   61 |
| ThexenoWireInterface<TwoWire>         |   2264/  477 |  1808/  466 |
| TodbotWireInterface<SoftI2CMaster>    |   3068/  239 |  2612/  228 |
+--------------------------------------------------------------------+

```

### Sparkfun Pro Micro

* 16 MHz ATmega32U4
* Arduino IDE 1.8.13, Arduino CLI 0.19.2
* SparkFun AVR Boards 1.1.13

```
+--------------------------------------------------------------------+
| functionality                         |  flash/  ram |       delta |
|---------------------------------------+--------------+-------------|
| baseline                              |   3472/  151 |     0/    0 |
|---------------------------------------+--------------+-------------|
| SimpleWireInterface                   |   4428/  156 |   956/    5 |
| SimpleWireFastInterface               |   3728/  153 |   256/    2 |
|---------------------------------------+--------------+-------------|
| TwoWireInterface<TwoWire>             |   5910/  369 |  2438/  218 |
|---------------------------------------+--------------+-------------|
| FeliasFoggWireInterface<SlowSoftWire> |   5070/  223 |  1598/   72 |
| MarpleWireInterface<SoftWire>         |   5924/  275 |  2452/  124 |
| RaemondWireInterface<SWire>           |   4766/  225 |  1294/   74 |
| SeeedWireInterface<SoftwareI2C>       |   4644/  161 |  1172/   10 |
|---------------------------------------+--------------+-------------|
| TestatoWireInterface<SoftwareWire>    |   5388/  212 |  1916/   61 |
| ThexenoWireInterface<TwoWire>         |   5322/  617 |  1850/  466 |
| TodbotWireInterface<SoftI2CMaster>    |   6150/  379 |  2678/  228 |
+--------------------------------------------------------------------+

```

### STM32 Blue Pill

* STM32F103C8, 72 MHz ARM Cortex-M3
* Arduino IDE 1.8.19, Arduino CLI 0.19.2
* STM32duino 2.2.0

```
+--------------------------------------------------------------------+
| functionality                         |  flash/  ram |       delta |
|---------------------------------------+--------------+-------------|
| baseline                              |  21880/ 3540 |     0/    0 |
|---------------------------------------+--------------+-------------|
| SimpleWireInterface                   |  24672/ 3568 |  2792/   28 |
|---------------------------------------+--------------+-------------|
| TwoWireInterface<TwoWire>             |  29164/ 3752 |  7284/  212 |
|---------------------------------------+--------------+-------------|
| FeliasFoggWireInterface<SlowSoftWire> |  25552/ 3620 |  3672/   80 |
| MarpleWireInterface<SoftWire>         |  25880/ 3692 |  4000/  152 |
| RaemondWireInterface<SWire>           |  25088/ 3636 |  3208/   96 |
| SeeedWireInterface<SoftwareI2C>       |  24780/ 3580 |  2900/   40 |
+--------------------------------------------------------------------+

```

### ESP8266

* NodeMCU 1.0, 80MHz ESP8266
* Arduino IDE 1.8.19, Arduino CLI 0.19.2
* ESP8266 Boards 3.0.2

```
+--------------------------------------------------------------------+
| functionality                         |  flash/  ram |       delta |
|---------------------------------------+--------------+-------------|
| baseline                              | 260089/27892 |     0/    0 |
|---------------------------------------+--------------+-------------|
| SimpleWireInterface                   | 261521/28000 |  1432/  108 |
|---------------------------------------+--------------+-------------|
| TwoWireInterface<TwoWire>             | 264485/28384 |  4396/  492 |
|---------------------------------------+--------------+-------------|
| FeliasFoggWireInterface<SlowSoftWire> | 263513/28056 |  3424/  164 |
| MarpleWireInterface<SoftWire>         | 263753/28128 |  3664/  236 |
| RaemondWireInterface<SWire>           | 262133/28076 |  2044/  184 |
| SeeedWireInterface<SoftwareI2C>       | 261693/28008 |  1604/  116 |
+--------------------------------------------------------------------+

```

### ESP32

* ESP32-01 Dev Board, 240 MHz Tensilica LX6
* Arduino IDE 1.8.19, Arduino CLI 0.19.2
* ESP32 Boards 2.0.2

```
+--------------------------------------------------------------------+
| functionality                         |  flash/  ram |       delta |
|---------------------------------------+--------------+-------------|
| baseline                              | 204501/16060 |     0/    0 |
|---------------------------------------+--------------+-------------|
| SimpleWireInterface                   | 207621/16236 |  3120/  176 |
|---------------------------------------+--------------+-------------|
| TwoWireInterface<TwoWire>             | 231965/16940 | 27464/  880 |
|---------------------------------------+--------------+-------------|
| FeliasFoggWireInterface<SlowSoftWire> | 209825/16308 |  5324/  248 |
| MarpleWireInterface<SoftWire>         | 209989/16372 |  5488/  312 |
| RaemondWireInterface<SWire>           | 208561/16308 |  4060/  248 |
| SeeedWireInterface<SoftwareI2C>       | 208357/16260 |  3856/  200 |
+--------------------------------------------------------------------+

```

### Teensy 3.2

* 96 MHz ARM Cortex-M4
* Arduino IDE 1.8.19, Arduino CLI 0.19.2
* Teensyduino 1.56
* Compiler options: "Faster"

```
+--------------------------------------------------------------------+
| functionality                         |  flash/  ram |       delta |
|---------------------------------------+--------------+-------------|
| baseline                              |  10216/ 4152 |     0/    0 |
|---------------------------------------+--------------+-------------|
| SimpleWireInterface                   |  11736/ 4160 |  1520/    8 |
|---------------------------------------+--------------+-------------|
| TwoWireInterface<TwoWire>             |  14400/ 4984 |  4184/  832 |
|---------------------------------------+--------------+-------------|
| FeliasFoggWireInterface<SlowSoftWire> |  11932/ 4212 |  1716/   60 |
| MarpleWireInterface<SoftWire>         |  12248/ 4280 |  2032/  128 |
| RaemondWireInterface<SWire>           |  11796/ 4228 |  1580/   76 |
| SeeedWireInterface<SoftwareI2C>       |  11800/ 4172 |  1584/   20 |
+--------------------------------------------------------------------+

```

