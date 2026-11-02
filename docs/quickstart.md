# Quickstart

Read a gas flow from your MCU: wire four signals, write a small port class, call three driver functions.

## 1. Wire it

The SFM4300 has a 6-pin JST NSH connector (1.0 mm pitch).

| MCU | SFM4300 pin | Notes |
| --- | --- | --- |
| I2C SDA | 2 (SDA) | Pull-up to the MCU's I/O voltage (2.2 kΩ to 4.7 kΩ is typical) |
| I2C SCL | 5 (SCL) | Pull-up as SDA; 400 kHz is the typical clock, 1 MHz the maximum |
| 3.3 V | 4 (VDD) | 3.0 V to 5.5 V; the sensor is calibrated at 3.3 V |
| GND | 3 (GND) | Common ground |
| (none) | 1 (ADDR) | Leave open or tie to GND for address 0x2A; never tie it to VDD |
| GPIO in (optional) | 6 (IRQn) | Low when a sample is ready; leave it unconnected if unused |

The [Hardware](hardware.md) page has the other addresses, the electrical limits and the timing.

## 2. Add the driver

Add the repository as a submodule and link the `hf::sfm` CMake target (or add `inc/` to your include path and
build with C++17). [Installation](installation.md) covers CMake and ESP-IDF.

```bash
git submodule add https://github.com/hardfoc/hf-sfm-flow-meter-driver.git external/hf-sfm-flow-meter-driver
```

## 3. Write the port

The port is the only platform code. It writes and reads raw bytes to a 7-bit address and waits. Replace the
`platform_*` calls with your SDK's.

```cpp title="my_i2c.hpp"
--8<-- "tests/docs/my_i2c.hpp:port"
```

`Read()` must return `false` when the sensor NACKs its address: that is how the SFM4300 says "no new sample
yet". [Porting](porting.md) explains each method.

## 4. Read a flow

```cpp title="main.cpp"
--8<-- "tests/docs/quickstart.cpp:quickstart"
```

What each step does:

1. `ReadProductIdentifier()` reads the 32-bit product number and serial number and decodes the variant
   (for example SFM4300-20-P). The sensor answers it only in idle mode, so call it before starting.
2. `StartContinuous()` reads the scale factor, offset and unit for the gas, sends the start command and waits
   the 12 ms until the first result is ready.
3. `ReadMeasurement()` reads one 9-byte frame (flow, temperature, status), checks three CRCs and converts the flow
   to standard litres per minute (slm, at 20 °C and 1013.25 mbar).

Results are accurate after about 30 ms of measuring: discard the first readings. Poll at any rate up to 2 kHz; in
the default averaging mode each read returns the mean of the samples since the previous read.

This example is built by the host CI, so it compiles against the current headers.

## If it does not work

| Symptom | First thing to check |
| --- | --- |
| `ReadProductIdentifier()` returns `NoData` or `BusWrite` | Pull-ups, the address (ADDR strap), SDA and SCL swapped, the sensor powered |
| It fails only after a start | The sensor takes commands only in idle mode: call `Stop()` first |
| `ReadMeasurement()` always returns `NoData` | No start command was sent, or the poll is faster than the sample rate |
| Occasional `Crc` errors | Weak pull-ups or long wires; lower the clock to 100 kHz ([Troubleshooting](troubleshooting.md)) |

## Next

- [API guide](api-guide.md): gases and mixtures, averaging, sleep, errors.
- [Measurement and averaging](measurement.md): how often to read and what a read returns.
- [Examples](examples.md): complete ESP32-S3 applications.
