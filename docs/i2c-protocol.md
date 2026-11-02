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

| Command | Code | Argument | Driver call | Notes |
| --- | --- | --- | --- | --- |
| Start, O<sub>2</sub> | `0x3603` | | `StartContinuous(Gas::O2)` | Gas 0 |
| Start, air | `0x3608` | | `StartContinuous(Gas::Air)` | Gas 1 |
| Start, N<sub>2</sub>O | `0x3615` | | `StartContinuous(Gas::N2O)` | Gas 2, SFM4300-20 only |
| Start, CO<sub>2</sub> | `0x361E` | | `StartContinuous(Gas::CO2)` | Gas 3, SFM4300-20 only |
| Start, air / O<sub>2</sub> | `0x3632` | O<sub>2</sub> ‰ | `StartContinuous(Gas::AirO2Mix, o2)` | Mixture 0 |
| Start, N<sub>2</sub>O / O<sub>2</sub> | `0x3639` | O<sub>2</sub> ‰ | `StartContinuous(Gas::N2OO2Mix, o2)` | Mixture 1, SFM4300-20 only |
| Start, CO<sub>2</sub> / O<sub>2</sub> | `0x3646` | O<sub>2</sub> ‰ | `StartContinuous(Gas::CO2O2Mix, o2)` | Mixture 2, SFM4300-20 only |
| Transmit concentration | `0xE17D` | O<sub>2</sub> ‰ | `UpdateConcentration()` | While measuring; then reset the pointer |
| Reset I2C address pointer | `0xE000` | | `UpdateConcentration()` | Back to the result buffer |
| Stop continuous measurement | `0x3FF9` | | `Stop()` | Idle after up to 0.5 ms |
| Configure averaging | `0x366A` | N | `ConfigureAveraging()` | Idle only; 0 = average-until-read, 1 to 128 = fixed N |
| Read scale factor, offset, unit | `0x3661` | start code | `ReadScaleOffsetUnit()` | Idle only; three words back |
| Enter sleep | `0x3677` | | `EnterSleep()` | Idle only |
| Exit sleep | (address + W) | | `ExitSleep()` | Not acknowledged; poll until the address ACKs (typically 16 ms) |
| Read product identifier | `0xE102` | | `ReadProductIdentifier()` | Idle only; six words back |
| Soft reset | `0x06` to address `0x00` | | `SoftReset()` | General call; about 16 ms, 20 ms maximum |

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

| Signal | SFM4300-20 | SFM4300-50 | Unit |
| --- | --- | --- | --- |
| Flow, every calibrated gas and mixture | scale 2500, offset -28672 | scale 1000, offset -28672 (O<sub>2</sub>, air, air/O<sub>2</sub>) | slm |
| Temperature | scale 200, offset 0 | scale 200, offset 0 | °C |

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

| Start code | `0x3603` | `0x3608` | `0x3615` | `0x361E` | `0x3624` | `0x362F` | `0x3632` | `0x3639` | `0x3646` |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| Bits 15:12 | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 |

`sfm::StatusWord` decodes the fields: `command_nibble()`, `exponential_smoothing()`, `fixed_n_averaging()`,
`gas_fraction_permille()` and `pure_gas()`.

!!! warning "Mixture codes in `GasFromStatusNibble()`"
    For SF06 parts, `sfm::GasFromStatusNibble()` maps 4, 5 and 6 to the three mixtures. The datasheet assigns 4
    and 5 to the reserved codes and 6, 7 and 8 to the mixtures. Pure gases (0 to 3) decode correctly. Until the
    function is fixed, use `ActiveGas()` to know which mixture is running.

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
