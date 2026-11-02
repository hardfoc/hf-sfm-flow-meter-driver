# Changelog

All notable changes to this driver are listed here. The format follows
[Keep a Changelog](https://keepachangelog.com/en/1.1.0/) and the project uses
[Semantic Versioning](https://semver.org/).

## [Unreleased]

### Added

- CI workflow that builds and runs the host unit tests with GCC and Clang.

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
