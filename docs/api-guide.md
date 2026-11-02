# API guide

How to do the common jobs with the driver: identify the part, start a gas or a mixture, read, average, sleep and
handle errors.

Every snippet on this page is compiled by the host CI from
[`tests/docs/api_tasks.cpp`](../tests/docs/api_tasks.cpp), with `Flow` standing for `sfm::Driver<MyI2c>`. The
[API reference](api-reference.md) lists every member.

## The shape of the API

One `sfm::Driver<I2cT>` object per sensor, constructed with your port, the sensor's address and optionally the
part family. Every command returns `sfm::DriverResult<T>`, never throws and never allocates.

| Task | Calls |
| --- | --- |
| Identify the part | `ReadProductIdentifier()`, `KnownVariant()`, `PartFamily()`, `AssumeVariant()` |
| Measure | `StartContinuous()`, `ReadMeasurement()`, `ReadFlowOnly()`, `Stop()` |
| Mixtures | `StartContinuous(gas, o2_permille)`, `UpdateConcentration()` |
| Configure | `ConfigureAveraging()`, `ReadScaleOffsetUnit()`, `SetAddress()` |
| Power | `EnterSleep()`, `ExitSleep()`, `SoftReset()` |
| State | `ActiveGas()`, `Measuring()`, `ActiveScaling()`, `Address()` |
| No bus access | `DecodeMeasurement()`, `EncodeWord()`, `DecodeWords()`, `sfm::Crc8()`, `sfm::StartCommandFor()`, `sfm::SupportsGas()` |
| SFx6000 only | `ReadTemperature()` |

A `DriverResult<T>` has `value` and `error`. It converts to `bool` (`true` when `error` is `None`), and
`sfm::ToString(error)` names the error for a log.

## Identify the part

```cpp
--8<-- "tests/docs/api_tasks.cpp:identify"
```

The sensor answers only in idle mode: before the first start, or after `Stop()`. The result sets the variant and
the family the driver uses for every later start. [Gases and part families](part-families.md) explains why that
matters.

## Start a measurement

```cpp
--8<-- "tests/docs/api_tasks.cpp:start"
```

`StartContinuous()` checks the gas against the known variant, reads the scale factor, offset and unit for that
gas (so the sensor must be idle), sends the start command and waits 12 ms for the first result. To change the gas,
call it again: when this driver started the running measurement (`Measuring()`), it sends `Stop()` first. A
measurement it did not start (the sensor kept measuring across an MCU reset) needs an explicit `Stop()`.

## Read

```cpp
--8<-- "tests/docs/api_tasks.cpp:read"
```

One read is one 9-byte transfer and three CRC checks. `NoData` is normal: the sensor NACKs a read while it has no
new result. Call it at your control-loop rate, up to 2 kHz; [Measurement and averaging](measurement.md) explains
what each read returns in each averaging mode.

When you need only the flow, read one word instead of three:

```cpp
--8<-- "tests/docs/api_tasks.cpp:flow-only"
```

`ReadFlowOnly()` does not refresh the status or the temperature.

## Measure a mixture

```cpp
--8<-- "tests/docs/api_tasks.cpp:mixture"
```

The argument is the O<sub>2</sub> volume fraction in per mille (0 to 1000); the driver refuses larger values with
`InvalidParameter` (the sensor would stop measuring). `UpdateConcentration()` changes it without the 12 ms restart
of a new start command. Send it at most once per millisecond, and do not read in between its two commands.

## Average on the sensor

```cpp
--8<-- "tests/docs/api_tasks.cpp:averaging"
```

Averaging is configured in idle mode and holds until the next reset. 0 is average-until-read (the default), 1 to
128 is fixed-N.

## Sleep and reset

```cpp
--8<-- "tests/docs/api_tasks.cpp:power"
```

Sleep needs idle mode and is SF06-only. The sensor does not acknowledge the wake-up write; `ExitSleep()` polls the
address in 5 ms steps for up to 20 ms. A soft reset is a general-call write that resets every device on the bus
that supports it; after it, averaging is back to average-until-read and the sensor is idle.

## Read from an interrupt or DMA

```cpp
--8<-- "tests/docs/api_tasks.cpp:decode"
```

`DecodeMeasurement()` is static and touches no bus: it checks the CRCs and scales the frame with the scaling of
the active gas. Read the frame when IRQn falls; see [Measurement and averaging](measurement.md#the-irqn-line).

## Handle errors

```cpp
--8<-- "tests/docs/api_tasks.cpp:errors"
```

| Error | Meaning | What to do |
| --- | --- | --- |
| `NoData` | A result-buffer read was NACKed: no new sample yet, the sensor is idle, or nothing answered | Skip the tick; if it persists, check the start and the address |
| `BusRead` | The reply of the identifier or scaling command was NACKed | Wiring, the address, or the sensor is measuring (these need idle mode) |
| `UnexpectedUnit` | The scaling reply cannot convert: scale factor 0, or a unit other than `0x0148` on a known SF06 part | The variant assumed does not match the part, or the reply is corrupt |
| `Crc` | A received word failed its CRC-8 | Count it; persistent errors mean noise, weak pull-ups or a port that returns partial reads |
| `BusWrite` | A command write was NACKed or failed | Address, wiring, or the sensor is measuring when the command needs idle |
| `NotInitialized` | The port's `EnsureInitialized()` returned `false` | Bus set-up in your port |
| `UnsupportedGas` | The known variant has no table for this gas | Use a gas from [the table](part-families.md#gas-tables-by-variant) |
| `VariantUnknown` | The gas's start code depends on the family and the part is not known | `ReadProductIdentifier()` or `AssumeVariant()` first |
| `InvalidParameter` | Averaging over 128, O<sub>2</sub> fraction over 1000 ‰ | Fix the argument |
| `NotSupported` | The command does not exist on this family, or the port has no general-call write | See [Gases and part families](part-families.md) |

## SFx6000 temperature

```cpp
--8<-- "tests/docs/api_tasks.cpp:sfx6000"
```

On SFx6000 parts the second word of the measurement frame is reserved, so `Measurement::temperature_valid` is
`false`; `ReadTemperature()` moves the output-buffer pointer, reads one word and moves it back. On SF06 parts it
returns `NotSupported`: the temperature is in every frame.

Next: [Configuration](configuration.md).
