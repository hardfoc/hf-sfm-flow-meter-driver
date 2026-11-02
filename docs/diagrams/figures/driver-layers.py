"""The three layers of the driver: hardware-agnostic core, communication interface, platform port.

Reads: From top to bottom. Your application talks to one sfm::Driver<I2c> object per sensor, whose public commands
are listed from inc/sfm_driver.hpp. The core turns each call into 16-bit SF06 commands, checks the CRC-8 of every
word it reads and converts raw counts with the scaling the sensor reported. The communication interface
(sfm::I2cInterface in inc/sfm_i2c_interface.hpp) is a CRTP base: four methods are required, general call is
optional. The platform port is the only code you write. Below it is the sensor.
"""

import re
from pathlib import Path

from hardfoc_mkdocs_theme.figkit import Fig

ROOT = Path(__file__).resolve().parents[3]
DRIVER = "inc/sfm_driver.hpp"
COMM = "inc/sfm_i2c_interface.hpp"


def data():
    drv = (ROOT / DRIVER).read_text()
    public = drv.split("private:")[0]
    commands = re.findall(r"^    DriverResult<[^>]+> (\w+)\(", public, re.M)
    comm = (ROOT / COMM).read_text()
    required = re.findall(r"static_cast<Derived\*>\(this\)->(\w+)\(", comm)
    optional = re.findall(r"static_cast<Derived\*>\(this\)->(\w+Impl)\(", comm)
    required = [n for n in dict.fromkeys(required) if n not in optional]
    return {
        "commands": commands,
        "required": required,
        "optional": list(dict.fromkeys(optional)),
        "sources": [f"{DRIVER}:Driver public commands", f"{COMM}:I2cInterface"],
    }


def build(d):
    f = Fig(
        "driver-layers",
        "Three layers, one of them yours",
        alt="Layer diagram: the application calls the hardware-agnostic sfm::Driver and its commands, which use the "
        "CRTP I2C interface, which calls the platform port you write, which drives the I2C bus of the Sensirion "
        "SFM4300 or another SF06-family flow meter.",
        width=1200,
        kicker="Architecture",
        subtitle="The driver and the interface are the same on every MCU. The port is a small class with four "
        "required methods.",
        profile="overview",
        legend=[("brand", "this library"), ("teal", "you write"), ("slate", "hardware")],
    )
    W = f.body_w
    app = f.card(0, 0, W, "Your application", ["control loop, gas mixer, ventilator, leak test ..."], tone="slate")
    top = app.b + 40
    chips_h = f.chips(16, f.region_inner_top(top), W - 32, d["commands"], tone="brand")
    reg = f.region(0, top, W, f.region_inner_top(top) - top + chips_h + 18, "Hardware-agnostic core", tone="brand",
                   sub="sfm::Driver<I2c>, inc/sfm_driver.hpp")
    comm = f.card(0, reg.b + 40, W, "Communication interface",
                  ["sfm::I2cInterface<Self>: CRTP, no virtual calls; framing, CRC-8 checks, part family and",
                   "scaling live in the core above, so the interface only moves raw bytes to a 7-bit address"],
                  tone="brand", accent=True)
    port = f.card(0, comm.b + 40, W, "Platform port (your class)",
                  ["required:  " + "  ".join(n + "()" for n in d["required"]),
                   "optional:  " + "  ".join(n + "()" for n in d["optional"])],
                  tone="teal", accent=True, mono=True)
    hw = f.card(0, port.b + 40, W, "SFM4300 / SF06 flow meter",
                ["I2C up to 1 MHz, address 0x2A-0x2D (ADDR resistor)  /  IRQn active low, optional"], tone="slate")
    for a, b in ((app, reg), (reg, comm), (comm, port), (port, hw)):
        f.connect(a, b, "bottom", "top", tone="rail")
    return f
