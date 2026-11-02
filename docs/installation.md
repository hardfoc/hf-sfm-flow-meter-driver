# Installation

Add the driver to a CMake or ESP-IDF project. It is a header-only C++17 library with no dependencies.

## Requirements

- A C++17 compiler: GCC 9+, Clang 10+ or the ESP-IDF toolchains.
- CMake 3.16 or newer, if you use the CMake target.
- An I2C controller that can do raw writes and reads to a 7-bit address (no register byte), and, for
  `SoftReset()`, a one-byte write to the general-call address 0x00.
- No RTOS, heap or exceptions: every call is `noexcept` and returns `sfm::DriverResult<T>`.

## Get the source

A git submodule keeps the driver pinned to a version you chose:

```bash
git submodule add https://github.com/hardfoc/hf-sfm-flow-meter-driver.git external/hf-sfm-flow-meter-driver
git submodule update --init external/hf-sfm-flow-meter-driver
```

Only `inc/`, `cmake/` and `CMakeLists.txt` are needed to build. The examples have their own submodule
(`examples/esp32/scripts`); you do not need it to use the driver.

## CMake

```cmake
add_subdirectory(external/hf-sfm-flow-meter-driver)
target_link_libraries(my_app PRIVATE hf::sfm)
```

`hf::sfm` (the `hf_sfm` target) is an `INTERFACE` library: it carries the include directories, including the
generated `sfm_version.h`, and requires C++17 (`cxx_std_17`).

| Option | Default | Effect |
| --- | --- | --- |
| `HF_SFM_ENABLE_WARNINGS` | `OFF` | Adds `-Wall -Wextra -Wpedantic` to targets that link `hf::sfm` |
| `HF_SFM_BUILD_TESTS` | `OFF` | Builds the host unit tests and the documentation snippets in `tests/` ([Testing and CI](testing.md)) |

### Install and `find_package`

```bash
cmake -S external/hf-sfm-flow-meter-driver -B build-sfm
cmake --install build-sfm --prefix /opt/hardfoc
```

This installs the headers and `sfm_version.h` under `include/hf_sfm/` and the package files under
`lib/cmake/hf_sfm/`. A project then finds it with `CMAKE_PREFIX_PATH` pointing at the prefix:

```cmake
find_package(hf_sfm 1.0 REQUIRED CONFIG)
target_link_libraries(my_app PRIVATE hf::sfm)
```

The host build checks this: the `hf_sfm_install_consumer` test installs into a scratch prefix and builds a
consumer against it.

### Settings file only

Without `add_subdirectory`, include the build settings and use their include list:

```cmake
include(external/hf-sfm-flow-meter-driver/cmake/hf_sfm_build_settings.cmake)
target_include_directories(my_app PRIVATE ${HF_SFM_PUBLIC_INCLUDE_DIRS})
target_compile_features(my_app PRIVATE cxx_std_17)
```

## ESP-IDF

The ESP-IDF component wrapper is
[`examples/esp32/components/hf_sfm/`](../examples/esp32/components/hf_sfm/CMakeLists.txt). Copy that folder into
your project's `components/` (or point `EXTRA_COMPONENT_DIRS` at it) and set the `HF_SFM_ROOT` cache variable to
the driver checkout; the wrapper stops with a clear message when the path is wrong. Your component then declares
`REQUIRES hf_sfm`. The wrapper requires the IDF `driver` component, which the example I2C port uses.

## How the build is organised

| Layer | File | Role |
| --- | --- | --- |
| Settings | [`cmake/hf_sfm_build_settings.cmake`](../cmake/hf_sfm_build_settings.cmake) | Include directories, version, generated header: the single source of truth |
| Library | [`CMakeLists.txt`](../CMakeLists.txt) | `INTERFACE` target `hf::sfm`, tests option, install rules and package config |
| ESP-IDF | [`examples/esp32/components/hf_sfm/CMakeLists.txt`](../examples/esp32/components/hf_sfm/CMakeLists.txt) | IDF component wrapper over the same settings |

The version is set once in the settings file and written into `sfm_version.h` at configure time. Read it at run
time:

```cpp
--8<-- "tests/docs/configuration.cpp:version"
```

## Headers

| Header | Contents |
| --- | --- |
| `sfm.hpp` | Umbrella: includes the four below and `sfm_version.h`; `sfm::GetDriverVersion()` |
| `sfm_driver.hpp` | `sfm::Driver<I2cT>` |
| `sfm_i2c_interface.hpp` | `sfm::I2cInterface<Derived>`, the CRTP base of your port |
| `sfm_types.hpp` | `DriverResult`, `DriverError`, `Family`, `Gas`, `Variant`, `Measurement`, `Scaling`, `StatusWord` |
| `sfm_commands.hpp` | Addresses, command codes, timing constants, `Crc8()` |

## Next

[Quickstart](quickstart.md) wires the sensor and reads a flow; [Porting](porting.md) writes the I2C interface for
your MCU.
