# Gases and part families

Which gas tables each part carries, why the driver needs to know the part before it starts some gases, and how
the SF06 and SFx6000 families differ.

## Gas tables by variant

A thermal flow sensor is calibrated per gas. Measuring CO<sub>2</sub> with the air table gives a reading that is
plausible and wrong. The driver therefore refuses a gas the known variant is not calibrated for, with
`DriverError::UnsupportedGas`.

| Variant | Range | Calibrated gases |
| --- | --- | --- |
| SFM4300-20-B, -O, -P | 0 to 20 slm | O<sub>2</sub>, air, N<sub>2</sub>O, CO<sub>2</sub>, air/O<sub>2</sub>, N<sub>2</sub>O/O<sub>2</sub>, CO<sub>2</sub>/O<sub>2</sub> |
| SFM4300-50-B, -O, -P | 0 to 50 slm | O<sub>2</sub>, air, air/O<sub>2</sub> |
| SFM6000D-50, -20, -5 | 50, 20, 5 slm (air, O<sub>2</sub>) | O<sub>2</sub>, air, N<sub>2</sub>O, CO<sub>2</sub>, Ar, air/O<sub>2</sub> |
| Unknown | | O<sub>2</sub>, air |

The SFM4300 rows come from the SFM4300 datasheet (sections 2.1 and 4.5). `sfm::SupportsGas()` encodes the table;
`sfm::FullScaleSlm()` gives the air / O<sub>2</sub> full scale of a known variant.

## Why the part has to be known

The two families share the framing and most command codes, but two start codes mean different gases:

| Start code | SF06 (SFM4300, SFM3xxx) | SFx6000 (SFM6000D) |
| --- | --- | --- |
| `0x3603` | O<sub>2</sub> | O<sub>2</sub> |
| `0x3608` | Air | Air |
| `0x3615` | N<sub>2</sub>O | CO<sub>2</sub> |
| `0x361E` | CO<sub>2</sub> | N<sub>2</sub>O |

So until the driver knows the family, it starts only O<sub>2</sub> and air, the two codes that mean the same gas
on both. Any other gas returns `DriverError::VariantUnknown`. Learn the part in one of two ways:

- `ReadProductIdentifier()`, in idle mode: it decodes the variant and sets the family.
- `AssumeVariant()`, when the identifier cannot be read (the sensor is already measuring) and the board fixes the
  part ([Configuration](configuration.md#tell-the-driver-which-part-is-fitted)).

The constructor's `family` argument only sets the assumption used before either of those.

Never pick a start code yourself: `sfm::StartCommandFor(family, gas)` returns the right one, and a
`static_assert` in the header pins the CO<sub>2</sub> codes of both families.

## SF06 and SFx6000 compared

| | SF06 | SFx6000 |
| --- | --- | --- |
| Parts | SFM4300, SFM3003, SFM3013, SFM3019, SFM3119 | SFM6000D |
| Measurement frame | flow, temperature, status | flow, reserved, status |
| Temperature | In every frame | `ReadTemperature()`, through an output-buffer pointer |
| Sample period | 0.5 ms | 1 ms |
| Scaling read | Three words | Five words through pointer `0xE151`: adds full-scale flow and the SEMI gas ID |
| Averaging, sleep | Yes | No: `ConfigureAveraging()`, `EnterSleep()`, `ExitSleep()` return `NotSupported` |
| Soft-reset wait | 20 ms | 30 ms |
| Default address | `0x2A` (SFM4300) | `0x24` |

The SFx6000 values come from Sensirion's SFC6xxx / SFM6xxx I2C interface reference (v1.1), as implemented in
[`inc/sfm_commands.hpp`](../inc/sfm_commands.hpp). The SFM6000D product identifiers are preliminary in that
document; the driver decodes `0x060211xx`, `0x060212xx` and `0x060214xx`.

## Other SF06 parts

SFM3003, SFM3013, SFM3019 and SFM3119 speak the same command set at their own addresses (`sfm::addr`). The driver
reads their scaling from the sensor, so readings are right, but it has no `Variant` for them and does not decode
their product identifiers: their variant stays `Unknown`, which limits them to O<sub>2</sub> and air. Pass the
part's address to the constructor (for example `sfm::addr::kSfm3019`).

Next: [API guide](api-guide.md).
