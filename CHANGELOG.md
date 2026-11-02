# Changelog

All notable changes to this driver are listed here. The format follows
[Keep a Changelog](https://keepachangelog.com/en/1.1.0/) and the project uses
[Semantic Versioning](https://semver.org/).

## [Unreleased]

### Breaking

- `ReadProductIdentifier()` and `ReadScaleOffsetUnit()` (and so `StartContinuous()`) return
  `DriverError::BusRead` instead of `NoData` when the command reply is NACKed (SFM4300 datasheet sections 4.3.6
  and 4.3.9). Result-buffer reads still return `NoData`.
- `StartContinuous()` returns `DriverError::UnexpectedUnit` and does not start when the scaling reply has scale
  factor 0, or a unit other than `0x0148` on a known SF06 part (section 4.3.6). It used to start and convert every
  reading to 0 or to the wrong unit.
- `GasFromStatusNibble(Family::Sf06, n)` maps 6, 7, 8 to the air/O2, N2O/O2, CO2/O2 mixtures (section 4.3.2
  Table 5). It mapped 4, 5, 6; 4 and 5 are reserved and now decode as `Gas::Air`.
- `Gas` and `Variant` have new enumerators (appended, values unchanged): a `switch` without `default` over them
  now warns with `-Wswitch`.
- `Stop()` waits 1 ms after the stop command (section 4.3.4: idle after up to 0.5 ms).

### Added

- CI workflow that builds and runs the host unit tests with GCC and Clang.
- `Variant` values for the SFM3003-300-CL / -CE, SFM3013-300-CL / -CLM, SFM3019 and SFM3119-240-CL, decoded from
  their product numbers; their gas tables in `SupportsGas()` and `FullScaleSlm()`. `addr::kSfm3003Ce` (`0x2D`).
- `Gas::HeOx` and `Gas::HeOxO2Mix` for the SFM3013-300-CLM, and a `GasFromStatusNibble(Variant, nibble)`
  overload that decodes them.
- `StatusNibbleIsKnown(family, nibble)` and `timing::kStopToIdleMs`.
- CMake install rules: headers under `include/hf_sfm`, exported `hf::sfm`, `find_package(hf_sfm)`; a CTest test
  that installs and builds a consumer.

### Fixed

- `StartContinuous()` while this driver's measurement is running sends `Stop()` first; the sensor accepts no other
  command while measuring (section 4.3.4), so switching gas used to fail.
- The `sfm_flow_demo` example configured averaging after the start, which the sensor refuses (averaging is
  idle-only, section 4.3.5); it now configures it first. Same order fix in the `sfm_driver.hpp` example.

### Changed

- Documentation moved to the HardFOC MkDocs platform: new page set (quickstart, hardware, architecture, I2C
  protocol, measurement and averaging, gases and part families, API guide, configuration, porting, testing), code
  snippets compiled by the host build from `tests/docs/`, Doxygen under `/api/`, versioned with mike. The Jekyll
  site files and the vendored doxygen-awesome stylesheet are gone.
- The host tests are built from `tests/CMakeLists.txt` (still `-DHF_SFM_BUILD_TESTS=ON`), which also compiles the
  documentation snippets.

## 1.0.0 (version in the build settings; not tagged)

- SF06 command set (SFM4300 datasheet section 4): continuous measurement per gas and O2 mixture, measurement and
  flow-only reads, averaging, concentration update, scale / offset / unit, sleep, product identifier, general-call
  reset, CRC-8 on every word.
- SFx6000 family (SFM6000D) beside SF06, with family-safe gas start codes.
- Host unit tests and ESP32-S3 examples.
