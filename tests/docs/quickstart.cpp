/**
 * @file quickstart.cpp
 * @brief The Quickstart page's example, compiled by the host build (never run).
 */
#include "my_i2c.hpp"

// --8<-- [start:quickstart]
#include "sfm.hpp"

int read_air_flow(MyI2c& i2c, float& slm) {
  sfm::Driver<MyI2c> flow(i2c, sfm::addr::kSfm4300Default);  // ADDR to GND: 0x2A

  const auto id = flow.ReadProductIdentifier();              // 1. identify (idle mode only)
  if (!id) {
    return -1;                                               // wiring, pull-ups or address
  }

  if (!flow.StartContinuous(sfm::Gas::Air)) {                // 2. read the scaling, start, wait 12 ms
    return -2;
  }

  const auto m = flow.ReadMeasurement();                     // 3. flow, temperature, status
  if (!m) {
    return m.error == sfm::DriverError::NoData ? 1 : -3;     // NoData: no new sample yet
  }
  slm = m.value.flow;                                        // standard litres per minute
  return 0;
}
// --8<-- [end:quickstart]
