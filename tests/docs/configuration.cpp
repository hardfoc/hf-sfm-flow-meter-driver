/**
 * @file configuration.cpp
 * @brief The Configuration page's snippets, compiled by the host build (never run).
 */
#include "my_i2c.hpp"

#include <cstdio>

bool two_sensors(MyI2c& i2c) {
  // --8<-- [start:address]
  // Two SFM4300 on one bus: ADDR to GND (0x2A) and ADDR to GND through 1.2 kOhm (0x2B).
  sfm::Driver<MyI2c> inlet(i2c, sfm::addr::kSfm4300Default);
  sfm::Driver<MyI2c> outlet(i2c, sfm::addr::kSfm4300Alt1);
  // --8<-- [end:address]
  return inlet.ReadProductIdentifier().ok() && outlet.ReadProductIdentifier().ok();
}

bool known_part(MyI2c& i2c) {
  // --8<-- [start:variant]
  // The sensor is already measuring (the identifier is idle-only), and the board fits an SFM4300-20-P.
  sfm::Driver<MyI2c> flow(i2c, sfm::addr::kSfm4300Default);
  flow.AssumeVariant(sfm::Variant::Sfm4300_20_P);  // also sets the family (SF06)
  if (!flow.Stop()) {                              // this driver did not start it, so stop it here
    return false;
  }
  return flow.StartContinuous(sfm::Gas::CO2).ok();
  // --8<-- [end:variant]
}

void print_version() {
  // --8<-- [start:version]
  std::printf("hf-sfm %s\n", sfm::GetDriverVersion());  // "1.0.0", from sfm_version.h
  // --8<-- [end:version]
}
