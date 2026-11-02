# Examples

Complete ESP32-S3 applications that read an SFM4300 with the driver, from a one-frame-per-second minimum to a
500 Hz loop with statistics.

They run on an ESP32-S3 with the sensor on I2C0 (SDA GPIO 8, SCL GPIO 9, 400 kHz, address 0x2A); the port is
[`hf_sfm_esp_i2c.hpp`](../examples/esp32/main/include/hf_sfm_esp_i2c.hpp). CI builds every app below on each pull
request.

## The apps

<!-- generated: examples -->
| App | What it shows | Source | CI |
| --- | --- | --- | --- |
| `sfm_minimal_example` :material-star:{ title="featured" } | Soft reset, product identifier, air table, one flow / temperature / status frame per second | [`sfm_minimal_example.cpp`](../examples/esp32/main/sfm_minimal_example.cpp) | yes |
| `sfm_flow_demo` :material-star:{ title="featured" } | 500 Hz poll with fixed-N averaging, CO2 table when the variant has it, else air; 1 s statistics | [`sfm_flow_demo.cpp`](../examples/esp32/main/sfm_flow_demo.cpp) | yes |

Build and flash one (from `examples/esp32`, ESP-IDF environment loaded):

```bash
./scripts/build_app.sh sfm_minimal_example Release
./scripts/flash_app.sh flash_monitor sfm_minimal_example Release
```

Source of this table: [`examples/esp32/app_config.yml`](../examples/esp32/app_config.yml).
<!-- /generated -->

## Set up once

```bash
git submodule update --init examples/esp32/scripts   # the HardFOC ESP-IDF build scripts
cd examples/esp32
. $IDF_PATH/export.sh                                 # ESP-IDF v5.5
./scripts/build_app.sh list                           # every app and build type
```

The scripts read [`app_config.yml`](../examples/esp32/app_config.yml) (the same file this page's table comes
from) and build each app as its own firmware image.

## What to read in an example

| You want to | Read |
| --- | --- |
| See the smallest complete program: install the bus, reset, identify, start, read | `sfm_minimal_example.cpp` |
| Pick a gas from the variant and fall back to air | `sfm_flow_demo.cpp`, gas selection |
| Run a fixed-rate loop and count `NoData` and `Crc` results | `sfm_flow_demo.cpp`, the 500 Hz loop |
| Write an ESP-IDF port | `include/hf_sfm_esp_i2c.hpp` |

The [example folder's README](../examples/esp32/README.md) has the wiring and the build commands.
