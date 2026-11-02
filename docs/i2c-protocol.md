# I2C protocol

The SF06 command set as the driver uses it: framing, every command, the frames the sensor returns, scaling and
the status word.

Source: [SFM4300 datasheet](https://sensirion.com/media/documents/024CBAA1/62987ADF/Sensirion_Datasheet_SFM4300.pdf),
section 4. The constants are in [`inc/sfm_commands.hpp`](../inc/sfm_commands.hpp).

## Framing

- A command is **16 bits**, MSB first, written without a register byte.
- A command with an argument adds a **16-bit argument and its CRC-8**: five bytes in all.
- A read returns **16-bit words, each followed by a CRC-8**. The host may stop after any word with a NACK and a
  STOP. The driver checks every CRC and returns `DriverError::Crc` on the first mismatch.
- The CRC-8 covers the two bytes before it: polynomial 0x31 (x⁸ + x⁵ + x⁴ + 1), initial value 0xFF, no
  reflection, no final XOR. Test vector: CRC(0xBEEF) = 0x92, checked at compile time by a `static_assert`.

## Commands

| Code | Command (argument) | Driver call, notes |
| --- | --- | --- |
| `0x3603` | Start, O<sub>2</sub> | `StartContinuous(Gas::O2)` |
| `0x3608` | Start, air | `StartContinuous(Gas::Air)` |
| `0x3615` | Start, N<sub>2</sub>O | `StartContinuous(Gas::N2O)`; SFM4300-20 only |
| `0x361E` | Start, CO<sub>2</sub> | `StartContinuous(Gas::CO2)`; SFM4300-20 only |
| `0x3632` | Start, air / O<sub>2</sub> (O<sub>2</sub> ‰) | `StartContinuous(Gas::AirO2Mix, o2)` |
| `0x3639` | Start, N<sub>2</sub>O / O<sub>2</sub> (O<sub>2</sub> ‰) | `StartContinuous(Gas::N2OO2Mix, o2)`; SFM4300-20 only |
| `0x3646` | Start, CO<sub>2</sub> / O<sub>2</sub> (O<sub>2</sub> ‰) | `StartContinuous(Gas::CO2O2Mix, o2)`; SFM4300-20 only |
| `0xE17D` | Transmit concentration (O<sub>2</sub> ‰) | `UpdateConcentration()`; while measuring |
| `0xE000` | Reset I2C address pointer | `UpdateConcentration()`, after `0xE17D` |
| `0x3FF9` | Stop continuous measurement | `Stop()`; idle after up to 0.5 ms, which `Stop()` waits |
| `0x366A` | Configure averaging (N) | `ConfigureAveraging()`; idle only; 0 = average-until-read, 1 to 128 = fixed N |
| `0x3661` | Read scale factor, offset, unit (start code) | `ReadScaleOffsetUnit()`; idle only; three words back |
| `0x3677` | Enter sleep | `EnterSleep()`; idle only |
| (address + W) | Exit sleep | `ExitSleep()`; not acknowledged, poll until the address ACKs (typically 16 ms) |
| `0xE102` | Read product identifier | `ReadProductIdentifier()`; idle only; six words back |
| `0x06` to `0x00` | Soft reset (general call) | `SoftReset()`; about 16 ms, 20 ms maximum |

Codes `0x3624` and `0x362F` are reserved on the SFM4300. While a continuous measurement runs, the sensor accepts
only stop, soft reset and the two concentration commands: stop it before anything else.

## Frames the sensor returns

| After | Words (each + CRC) | Bytes |
| --- | --- | --- |
| A start command | flow, temperature, status | 9 (3 for flow only) |
| `0x3661` | scale factor, offset, unit | 9 |
| `0xE102` | product number (2 words), serial number (4 words) | 18 |

Flow, temperature, scale factor and offset are signed 16-bit (two's complement); the unit and the status are
unsigned. A read header while no new sample exists is NACKed (`DriverError::NoData`).

## Scaling

The sensor reports raw counts. The physical value is `(raw - offset) / scale`, with the scale factor and offset of
the gas table in use:

| Signal | Scale factor | Offset | Unit |
| --- | --- | --- | --- |
| Flow, SFM4300-20 (every gas and mixture) | 2500 | -28672 | slm |
| Flow, SFM4300-50 (O<sub>2</sub>, air, air/O<sub>2</sub>) | 1000 | -28672 | slm |
| Temperature | 200 | 0 | °C |

The driver does not hard-code the flow scaling: `StartContinuous()` reads it from the sensor (`0x3661`) for the
gas you start, and `ReadMeasurement()` applies it. The temperature scale (200 per °C) is a constant,
`sfm::kTemperatureScalePerC`.

The flow unit is `0x0148` on every SFM4300 table: standard litres per minute at 20 °C and 1013.25 mbar
(bits 12:8 = 1 standard litre, bits 7:4 = 4 per minute, bits 3:0 = 8 prefix 10⁰).

## Status word

The third word of each measurement frame.

<div class="hf-registers" markdown>

| Bits | Name | Meaning |
| --- | --- | --- |
| `15:12` | running command | Index of the start command in the table below |
| `11` | exponential smoothing | 1: average-until-read has switched to exponential smoothing (no read for over 64 ms) |
| `10` | averaging mode | 0: average-until-read; 1: fixed-N averaging |
| `9:0` | gas concentration | O<sub>2</sub> volume fraction in ‰ for a mixture; `0x3FF` for a pure gas |

</div>

| Bits 15:12 | Start code | Measurement |
| --- | --- | --- |
| 0 | `0x3603` | O<sub>2</sub> |
| 1 | `0x3608` | Air |
| 2 | `0x3615` | N<sub>2</sub>O |
| 3 | `0x361E` | CO<sub>2</sub> |
| 4, 5 | `0x3624`, `0x362F` | Reserved |
| 6 | `0x3632` | Air / O<sub>2</sub> |
| 7 | `0x3639` | N<sub>2</sub>O / O<sub>2</sub> |
| 8 | `0x3646` | CO<sub>2</sub> / O<sub>2</sub> |

`sfm::StatusWord` decodes the fields: `command_nibble()`, `exponential_smoothing()`, `fixed_n_averaging()`,
`gas_fraction_permille()` and `pure_gas()`.

`sfm::GasFromStatusNibble(family, nibble)` turns bits 15:12 back into an `sfm::Gas` using this table (SF06) or
the SFx6000 one, and `sfm::StatusNibbleIsKnown(family, nibble)` tells a real code from a reserved one: a reserved
nibble decodes as `Gas::Air`. On the SFM3013-300-CLM, 2 and 7 are HeOx and HeOx / O<sub>2</sub>: the
`GasFromStatusNibble(variant, nibble)` overload decodes them.

## Product identifier

Bytes 1 to 4 of the `0xE102` reply form a 32-bit product number; the last 8 bits are a revision. The driver
ignores them when it decodes the variant.

| Product number | Variant | `sfm::Variant` |
| --- | --- | --- |
| `0x0403011x` | SFM4300-20-B | `Sfm4300_20_B` |
| `0x0403021x` | SFM4300-20-O | `Sfm4300_20_O` |
| `0x0403031x` | SFM4300-20-P | `Sfm4300_20_P` |
| `0x0403091x` | SFM4300-50-B | `Sfm4300_50_B` |
| `0x0403071x` | SFM4300-50-O | `Sfm4300_50_O` |
| `0x0403061x` | SFM4300-50-P | `Sfm4300_50_P` |

The datasheet marks `...8x` revisions as prototypes and `...1x` as production parts. The 64-bit serial number,
read as a decimal, has the form `yywwxxxxxx`: calibration year and week, then a sequence number.

Next: [Measurement and averaging](measurement.md).
