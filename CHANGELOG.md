# Changelog

* Unreleased
* 0.3.2 (2021-08-17)
    * Minor documentation updates.
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
