# SFM Flow Meter Driver API reference {#mainpage}

The classes, functions and constants of the HardFOC driver for Sensirion SF06-family gas mass flow meters
(SFM4300, SFM3003, SFM3013, SFM3019, SFM3119) and the SFM6000D, generated from the headers.

Start with sfm::Driver, the driver, and sfm::I2cInterface, the interface your platform port implements. Results
and errors are sfm::DriverResult and sfm::DriverError; readings are sfm::Measurement, sfm::Scaling and
sfm::StatusWord. Command codes, addresses and timing are in the sfm::cmd, sfm::addr and sfm::timing namespaces.

The guides, with examples that compile, are on the documentation site: <a href="../">SFM Flow Meter Driver
documentation</a>.
