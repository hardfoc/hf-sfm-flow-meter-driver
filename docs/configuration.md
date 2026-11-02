# Configuration

Everything you can set: the address, the part, the gas, the averaging, and the build options.

The driver has no configuration struct: each setting is a constructor argument or one call. Snippets are compiled
from [`tests/docs/configuration.cpp`](../tests/docs/configuration.cpp).

## Settings

| Setting | How | Default | Notes |
| --- | --- | --- | --- |
| I2C address | Constructor, `SetAddress()` | `0x2A` (`sfm::addr::kSfm4300Default`) | Must match the ADDR strap ([Hardware](hardware.md#i2c-address)) |
| Part family | Constructor `family` | `sfm::Family::Sf06` | Replaced once the variant is known |
| Variant | `ReadProductIdentifier()`, `AssumeVariant()` | `Unknown`: O<sub>2</sub> and air only | Decides which gases may start |
| Gas | `StartContinuous(gas)` | none | Per measurement |
| O<sub>2</sub> fraction | `StartContinuous(mix, o2_permille)`, `UpdateConcentration()` | 0 ‰ | Mixtures only, 0 to 1000 ‰ |
| Averaging | `ConfigureAveraging(n)` | 0: average-until-read | Idle only; 1 to 128 fixed-N; reset clears it |

## Several sensors on one bus

```cpp
--8<-- "tests/docs/configuration.cpp:address"
```

Up to four SFM4300 share a bus, one per ADDR strap. Each driver object only holds a reference to the port, so
they can share one port object. A soft reset is a general call: it resets every sensor on the bus at once.

## Tell the driver which part is fitted

```cpp
--8<-- "tests/docs/configuration.cpp:variant"
```

Use `AssumeVariant()` when the product identifier cannot be read, for example after an MCU reset while the sensor
kept measuring. It sets the family too. Assume only the part the board really carries: the gas check is the only
protection against reading CO<sub>2</sub> with a table the sensor does not have.

## Compile-time constants

These live in [`inc/sfm_commands.hpp`](../inc/sfm_commands.hpp) and follow the datasheet; you do not normally
change them.

| Constant | Value | Use |
| --- | --- | --- |
| `sfm::timing::kFirstResultAfterStartMs` | 12 | Wait in `StartContinuous()` |
| `sfm::timing::kAccuracySettleAfterStartMs` | 30 | Readings before this are a few percent off |
| `sfm::timing::kSoftResetMs` | 20 | Wait after an SF06 soft reset (16 ms typical) |
| `sfm::timing::kSfx6000SoftResetMs` | 30 | Wait after an SFx6000 soft reset |
| `sfm::timing::kMeasurementPeriodUs` | 500 | SF06 sample period |
| `sfm::timing::kMaxAveragingWindow` | 128 | Largest fixed-N window |
| `sfm::kTemperatureScalePerC` | 200 | Temperature counts per °C |
| `sfm::kFlowUnitSlm20C` | `0x0148` | The SFM4300's flow unit code |

## Build options

| Option | Default | Effect |
| --- | --- | --- |
| `HF_SFM_ENABLE_WARNINGS` | `OFF` | `-Wall -Wextra -Wpedantic` on targets that link `hf::sfm` |
| `HF_SFM_BUILD_TESTS` | `OFF` | Host unit tests and documentation snippets ([Testing and CI](testing.md)) |

Next: [Porting](porting.md).
