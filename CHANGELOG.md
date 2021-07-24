# Changelog

* Unreleased
    * Add `examples/MemoryBenchmark` and `examples/AutoBenchmark` to gather
      memory and CPU consumption numbers of the pre-installed hardware
      `<Wire.h>` and some selected third party I2C libaries:
        * https://github.com/Testato/SoftwareWire. (AVR only.)
        * https://github.com/RaemondBW/SWire
        * https://github.com/felias-fogg/SlowSoftWire
    * Add GitHub Actions validation.
    * Add support for "repeated start" by adding `sendStop` parameter to
      `endTransmission()`.
* 0.2 (2021-07-19)
    * Add ability to read from I2C device using `requestFrom()`, `read(),
      and `endRequest()`.
    * Finish most of the Usage Guide in README.md
* 0.1 (2021-06-25)
    * First GitHub release.
* 2021-06-24
    * Initial extraction from
      [AceSegment](https://github.com/bxparks/AceSegment) library.
