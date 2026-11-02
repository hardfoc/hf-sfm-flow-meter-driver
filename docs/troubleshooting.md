# Troubleshooting

Symptoms you may see while bringing up an SF06 flow meter, their causes and the fix for each.

Start with the error the call returned (`sfm::ToString(r.error)`): it separates bus problems (`BusWrite`,
`NoData` on every call) from data problems (`Crc`) and from configuration problems (`UnsupportedGas`,
`VariantUnknown`).

## Nothing answers

| Symptom | Cause | Fix |
| --- | --- | --- |
| Every call returns `BusWrite` or `NoData` | Wrong address | Check the ADDR strap: open or GND is `0x2A`; scan the bus |
| | No pull-ups, or SDA and SCL swapped | External pull-ups on both lines; check the connector pin order (SDA is pin 2, SCL pin 5) |
| | The sensor is still starting | Wait 20 ms after power-up before the first call |
| | ADDR tied to VDD | Not allowed: tie it to GND or leave it open |
| `NotInitialized` | The port's `EnsureInitialized()` failed | Your bus set-up (for the ESP32 port: call `Install()` first) |

## Reads fail

| Symptom | Cause | Fix |
| --- | --- | --- |
| `NoData` on every read | No start command, `Stop()` was sent, or the sensor was reset (IRQn low after a start) | Call `StartContinuous()` again |
| `NoData` on some reads | Polling faster than results arrive: 2 kHz, or 2 kHz / N with fixed-N averaging | Expected: skip the tick, or read on the IRQn edge |
| `Crc` now and then | Noise, slow edges, weak pull-ups, long cable | Stronger pull-ups, shorter wires, 100 kHz |
| `Crc` on every read | The port returns fewer bytes than asked, or reads with a register byte | Fix the port ([Porting](porting.md)) |

## Commands are refused

| Symptom | Cause | Fix |
| --- | --- | --- |
| `ReadProductIdentifier()` or `ConfigureAveraging()` fails, or `StartContinuous()` fails with a new gas | The sensor is measuring; it takes these only in idle mode | `Stop()`, wait 0.5 ms, then the command |
| `UnsupportedGas` | The variant has no table for the gas (for example CO<sub>2</sub> on an SFM4300-50) | Use a calibrated gas ([Gases and part families](part-families.md)) |
| `VariantUnknown` | The gas's start code depends on the family | `ReadProductIdentifier()` in idle mode, or `AssumeVariant()` |
| `NotSupported` from `SoftReset()` | The port has no `WriteGeneralCallImpl()` | Add it, or power-cycle the sensor |
| `NotSupported` from averaging or sleep | SFx6000 part | These commands exist only on SF06 parts |
| `ExitSleep()` returns `BusWrite` | The port cannot send an address-only write (`len == 0`) | Support empty writes in `Write()` |

Do not bypass `UnsupportedGas` with `AssumeVariant()`: a thermal sensor read with the wrong gas table gives a
plausible, systematically wrong flow.

## Readings look wrong

| Symptom | Cause | Fix |
| --- | --- | --- |
| The first readings are a few percent off | Warm-up: accuracy is reached about 30 ms after the start | Discard readings for 30 ms after each start |
| A reading lags a step change | Average-until-read smooths exponentially when you read less often than every 64 ms (status bit 11) | Read at least every 64 ms, or use fixed-N averaging |
| A constant offset | The sensor is not horizontal, or the line pressure is outside 0.66 bar to 7 bar absolute | Mount it horizontally with the connector up ([Hardware](hardware.md#fluidics)) |
| The temperature differs from the gas temperature | The sensor reports its silicon temperature | Expected; insulate the sensor if you need the gas temperature |
| The status shows the wrong mixture | `GasFromStatusNibble()` maps SF06 mixtures to 4 to 6; the datasheet uses 6 to 8 | Use `ActiveGas()` ([I2C protocol](i2c-protocol.md#status-word)) |
