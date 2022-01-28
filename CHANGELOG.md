# Changelog

* Unreleased
* 0.4.1 (2022-01-28)
    * Refactor README.md so that the compelling reason for using AceWire are
      the `SimpleWireInterface` and `SimpleWireFastInterface`.
        * Downgrade various `XxxInterface` wrapper classes for other I2C
          libraries as side benefits.
* 0.4.0 (2022-01-26)
    * Write custom wrapper classes for various third party I2C libraries.
        * `src/ace_wire/FeliasFoggWireInterface.h`
        * `src/ace_wire/SeeedWireInterface.h`
        * `src/ace_wire/TestatoWireInterface.h`
        * `src/ace_wire/MarpleWireInterface.h`
        * `src/ace_wire/ThexenoWireInterface.h`
        * `src/ace_wire/RaemondWireInterface.h`
        * `src/ace_wire/TodbotWireInterface.h`
    * Update various tool chains
        * Arduino IDE from 1.8.13 to 1.8.19
        * Arduino AVR from 1.8.3 to 1.8.4
        * STM32duino from 2.0.0 to 2.2.0
        * ESP32 from 1.0.6 to 2.0.2
        * Teensyduino from 1.55 to 1.56
    * Downgrade SAMD21 to "Tier 3: May work, but not supported".
    * Major rewrite of README.md.
        * Better organization.
        * More usage info on each XxxWireInterface class.
        * Better documentation of error handling.
* 0.3.2 (2021-08-17)
    * Minor documentation updates.
    * First public release.
* 0.3.1 (2021-08-10)
    * Update documentation of `T_WIRE` versus `T_WIREI`.
    * No functional change in this release.
* 0.3 (2021-07-30)
    * Add `examples/MemoryBenchmark` and `examples/AutoBenchmark` to gather
      memory and CPU consumption numbers of the pre-installed hardware
      `<Wire.h>` and some selected third party I2C libaries:
        * https://github.com/Testato/SoftwareWire. (AVR only.)
        * https://github.com/RaemondBW/SWire
        * https://github.com/felias-fogg/SlowSoftWire
        * https://github.com/Seeed-Studio/Arduino_Software_I2C
    * Add GitHub Actions validation.
    * Add support for "repeated start" by adding `sendStop` parameter to
      `endTransmission()`.
    * Support better error handling by returning a `uint8_t` status code from
      `beginTransmission()`.
    * Remove `endRequest()`. STOP condition is now sent by the `read()` method
      upon reading the last byte.
    * Add [examples/ReadWriteDemo](examples/ReadWriteDemo) with explicit error
      handling of each operation.
* 0.2 (2021-07-19)
    * Add ability to read from I2C device using `requestFrom()`, `read(),
      and `endRequest()`.
    * Finish most of the Usage Guide in README.md
* 0.1 (2021-06-25)
    * First GitHub release.
* 2021-06-24
    * Initial extraction from
      [AceSegment](https://github.com/bxparks/AceSegment) library.
