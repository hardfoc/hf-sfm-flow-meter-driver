---
hide:
  - navigation
  - toc
---

<div class="hf-hero" markdown>
<span class="hf-kicker">HardFOC driver</span>

# SFM Flow Meter Driver

<p>A header-only C++17 driver for Sensirion SF06-family gas mass flow meters such as the SFM4300. It identifies the
part, starts a measurement for the right gas or O<sub>2</sub> mixture and reads flow, temperature and status with
every word CRC-checked, on any MCU with I2C.</p>

[Quickstart](quickstart.md){ .md-button .md-button--primary }
[API guide](api-guide.md){ .md-button }
[Source on GitHub](https://github.com/hardfoc/hf-sfm-flow-meter-driver){ .md-button }
</div>

The driver has no platform code. You give it a small class that writes and reads raw bytes on I2C and waits; it
does the rest: command framing, CRC-8 checks, gas-table selection per part family, scaling to standard litres per
minute and status decoding.

<div class="hf-chip" markdown>
<div class="hf-chip__head">
<span class="hf-chip__kicker">Thermal mass flow meter for gas mixing</span>
<span class="hf-chip__name">SFM4300</span>
<span class="hf-chip__vendor">Sensirion (SF06 family)</span>
</div>

Measures
:   Gas mass flow, 0 to 20 slm (SFM4300-20) or 0 to 50 slm (SFM4300-50), and sensor temperature

Gases
:   O<sub>2</sub>, air, N<sub>2</sub>O, CO<sub>2</sub> and three O<sub>2</sub> mixtures (-20); O<sub>2</sub>, air
    and air/O<sub>2</sub> (-50)

Interface
:   I2C, 400 kHz typical, up to 1 MHz; address 0x2A to 0x2D (ADDR resistor); IRQn data-ready, active low

Supply
:   3.0 V to 5.5 V, calibrated at 3.3 V; 4.2 mA measuring, 0.4 µA asleep (typical)

Speed
:   New flow value about every 0.5 ms (2 kHz); step response τ<sub>63</sub> under 5 ms

Driver covers
:   SFM4300, SFM3003, SFM3013, SFM3019, SFM3119 (SF06 command set) and SFM6000D (SFx6000)

[Datasheet](https://sensirion.com/media/documents/024CBAA1/62987ADF/Sensirion_Datasheet_SFM4300.pdf) ·
[Sensirion reference driver](https://github.com/Sensirion/embedded-i2c-sfm-sf06)
</div>

## What you get

- **One object per sensor.** `sfm::Driver<YourI2c>` covers every command of the SF06 set: start and stop,
  read, averaging, concentration update, scaling, sleep, product identifier and soft reset.
  See the [API guide](api-guide.md).
- **The right gas table, or an error.** The driver reads the product identifier, knows which gases the variant
  is calibrated for and refuses the others with `UnsupportedGas`. See
  [Gases and part families](part-families.md).
- **Readings in slm.** Scale factor, offset and unit come from the sensor for the selected gas, so the
  conversion is right for every variant. See [I2C protocol](i2c-protocol.md#scaling).
- **Every word checked.** Each received 16-bit word carries a CRC-8 that the driver verifies; a NACK for "no new
  sample yet" is reported as `NoData`, not as a fault. See [Measurement and averaging](measurement.md).
- **Zero-overhead portability.** The port is a CRTP class with four methods: no virtual calls, no heap, no
  RTOS dependency. See [Porting](porting.md).
- **Tested without hardware.** Host unit tests replay datasheet frames through a fake I2C bus. See
  [Testing and CI](testing.md).

## Where to go next

<div class="grid cards" markdown>

-   :material-rocket-launch-outline: **[Quickstart](quickstart.md)**

    ---

    Wire the sensor, write the port, read a flow in about 15 lines.

-   :material-chip: **[Hardware](hardware.md)**

    ---

    Connector, addresses, bus settings, timing, power and fluidics.

-   :material-layers-outline: **[Architecture](architecture.md)**

    ---

    The three layers and what each one owns.

-   :material-code-braces: **[API guide](api-guide.md)**

    ---

    Identify, start, read, average and handle errors, task by task.

-   :material-swap-horizontal: **[Porting](porting.md)**

    ---

    Implement the I2C interface for your MCU.

-   :material-book-open-variant: **[API reference](api-reference.md)**

    ---

    Every class and member, generated from the headers.

</div>
