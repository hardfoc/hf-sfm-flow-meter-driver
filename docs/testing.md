# Testing and CI

How the driver is checked: host unit tests that replay datasheet frames, compiled documentation, ESP32 builds and
static analysis, on every pull request.

## Host unit tests

The tests build the real driver against a fake I2C transport. The fake records every write and replays scripted
read frames built from datasheet values (with their CRCs), or NACKs a read, so tests check exactly what goes over
the bus and how the driver decodes what comes back.

```bash
cmake -S . -B build-host -DHF_SFM_BUILD_TESTS=ON
cmake --build build-host
ctest --test-dir build-host --output-on-failure
```

| Test | Covers |
| --- | --- |
| `TestCrc` | CRC-8 test vector, word encoding with CRC |
| `TestProductIdentifier` | The `0xE102` command, 18-byte reply, variant and serial decoding |
| `TestStartAndMeasure` | Scaling read before the start, start code, the 12 ms wait, flow and temperature conversion, status decoding |
| `TestGasGating` | `UnsupportedGas` on an SFM4300-50, `VariantUnknown` before identification, mixture argument and CRC |
| `TestAveragingStopReset` | Averaging window limits, stop, general-call reset and its wait |
| `TestStatusNibbleMixtures` | Status bits 15:12 for the three SF06 mixtures (6 to 8) and the reserved codes 4 and 5 |
| `TestStopLatencyAndRestart` | The 0.5 ms wait after stop; a second start while measuring stops first |
| `TestSfm30xxVariants` | SFM3003 / 3013 / 3019 / 3119 product numbers, their gas tables, the SFM3013-300-CLM HeOx codes |
| `TestSfx6000Identity`, `TestSfx6000GasMapAndScaling`, `TestSfx6000Gating` | The SFx6000 family: identifier, swapped CO<sub>2</sub> / N<sub>2</sub>O codes, five-word scaling, temperature pointer, unsupported commands |

All tests are in [`tests/host/sfm_driver_test.cpp`](../tests/host/sfm_driver_test.cpp).

## Documentation snippets

Every C++ block on this site is included from [`tests/docs/`](../tests/docs/my_i2c.hpp), which the host build
compiles (target `hf_sfm_doc_snippets`, `-Wall -Wextra -Wpedantic`) with the driver instantiated over the
documented port. A change to the API that breaks a page breaks the build first. The README's example is a copy of
the Quickstart snippet that `hardfoc-docs check` keeps in step.

## CI workflows

| Workflow | What it checks |
| --- | --- |
| Host unit tests | GCC and Clang build and run the tests and the doc snippets |
| ESP32 examples build | Every app in `examples/esp32/app_config.yml`, for each listed ESP-IDF version and build type |
| C++ lint, C++ analysis | clang-format, clang-tidy and cppcheck |
| Docs | Strict site build, figure and generated-table drift, Doxygen, link check, Lighthouse budget |
| Markdown lint, YAML lint, link check | Repository text files |

The workflows call the shared, versioned workflows of
[hf-general-ci-tools](https://github.com/hardfoc/hf-general-ci-tools) and
[hf-espidf-ci-tools](https://github.com/hardfoc/hf-espidf-ci-tools).

## On hardware

The [ESP32 examples](examples.md) are the hardware check: `sfm_minimal_example` logs the product identifier,
the scaling and one frame per second; `sfm_flow_demo` reports `NoData` and `Crc` counts every second, which
should stay near zero on a healthy bus.
