/**
 * @file api_tasks.cpp
 * @brief The API guide's snippets, compiled by the host build (never run).
 */
#include "my_i2c.hpp"

#include <cstdint>
#include <cstdio>

using Flow = sfm::Driver<MyI2c>;

void use(float, float, bool) noexcept;
void use_flow(float) noexcept;

bool identify(Flow& flow) {
  // --8<-- [start:identify]
  const auto id = flow.ReadProductIdentifier();  // idle mode only: before StartContinuous() or after Stop()
  if (!id) {
    std::printf("no reply: %s\n", sfm::ToString(id.error).data());
    return false;
  }
  std::printf("product 0x%08lX, %s, %.0f slm full scale\n", static_cast<unsigned long>(id.value.product_id),
              sfm::ToString(id.value.variant).data(), static_cast<double>(sfm::FullScaleSlm(id.value.variant)));
  // --8<-- [end:identify]
  return true;
}

bool start(Flow& flow, MyI2c& i2c) {
  // --8<-- [start:start]
  if (!flow.ConfigureAveraging(0)) {               // idle only: average-until-read (the default)
    return false;
  }
  i2c.DelayMs(1);

  sfm::Gas gas = sfm::Gas::CO2;
  if (!sfm::SupportsGas(flow.KnownVariant(), gas)) {
    gas = sfm::Gas::Air;                           // SFM4300-50: O2, Air and Air/O2 only
  }
  if (const auto r = flow.StartContinuous(gas); !r) {
    std::printf("start failed: %s\n", sfm::ToString(r.error).data());
    return false;
  }
  const sfm::Scaling& s = flow.ActiveScaling();    // read from the sensor for this gas
  std::printf("scale %d, offset %d, unit 0x%04X\n", s.scale, s.offset, s.unit);
  // --8<-- [end:start]
  return true;
}

void read_tick(Flow& flow, uint32_t& errors) {
  // --8<-- [start:read]
  const auto m = flow.ReadMeasurement();
  if (m.error == sfm::DriverError::NoData) {
    return;                                        // no new sample since the last read: skip this tick
  }
  if (!m) {
    ++errors;                                      // Crc: noise, missing pull-ups, a partial read
    return;
  }
  const float slm = m.value.flow;                  // (raw - offset) / scale
  const float t_c = m.value.temperature_c;         // sensor silicon temperature, SF06 parts only
  const bool fixed_n = m.value.status.fixed_n_averaging();
  // --8<-- [end:read]
  use(slm, t_c, fixed_n);
}

void flow_only(Flow& flow) {
  // --8<-- [start:flow-only]
  const auto f = flow.ReadFlowOnly();              // 3 bytes instead of 9: no temperature, no status
  if (f) {
    use_flow(f.value);
  }
  // --8<-- [end:flow-only]
}

bool mixture(Flow& flow) {
  // --8<-- [start:mixture]
  if (!flow.StartContinuous(sfm::Gas::AirO2Mix, 400)) {  // 40.0 % O2 in air, in per mille (0 to 1000)
    return false;
  }
  // Later, while measuring: no restart, no 12 ms gap. At most once per millisecond.
  if (!flow.UpdateConcentration(600)) {                   // 60.0 % O2
    return false;
  }
  // --8<-- [end:mixture]
  return true;
}

bool averaging(Flow& flow, MyI2c& i2c) {
  // --8<-- [start:averaging]
  if (!flow.Stop()) {                              // the sensor takes commands only in idle mode
    return false;
  }
  i2c.DelayMs(1);                                  // up to 0.5 ms until it is idle
  if (!flow.ConfigureAveraging(4)) {               // fixed N = 4: one value every 2 ms (500 Hz)
    return false;
  }
  if (!flow.StartContinuous(sfm::Gas::Air)) {
    return false;
  }
  // --8<-- [end:averaging]
  return true;
}

bool power(Flow& flow, MyI2c& i2c) {
  // --8<-- [start:power]
  if (!flow.Stop()) {                              // continuous measurement to idle
    return false;
  }
  i2c.DelayMs(1);
  if (!flow.EnterSleep()) {                        // SF06 only, from idle: 0.4 uA typical
    return false;
  }
  // ... later
  if (!flow.ExitSleep()) {                         // a write header wakes it; polls until it ACKs
    return false;
  }
  if (const auto r = flow.SoftReset(); !r) {       // general call 0x06, then 20 ms (SFx6000: 30 ms)
    return false;                                  // NotSupported: the port has no WriteGeneralCallImpl()
  }
  // --8<-- [end:power]
  return true;
}

void decode(Flow& flow, const uint8_t (&frame)[9]) {
  // --8<-- [start:decode]
  // frame: the 9 bytes your interrupt or DMA handler read after IRQn went low
  const auto m = Flow::DecodeMeasurement(flow.PartFamily(), flow.ActiveScaling(), frame);
  if (m) {
    use_flow(m.value.flow);
  }
  // --8<-- [end:decode]
}

bool errors(Flow& flow) {
  // --8<-- [start:errors]
  const auto r = flow.StartContinuous(sfm::Gas::N2O);
  switch (r.error) {
    case sfm::DriverError::None:
      return true;
    case sfm::DriverError::VariantUnknown:         // N2O's start code means CO2 on SFx6000 parts
      (void)flow.ReadProductIdentifier();          // learn the family, then try again
      return flow.StartContinuous(sfm::Gas::N2O).ok();
    case sfm::DriverError::UnsupportedGas:         // this variant has no N2O table (SFM4300-50)
      return flow.StartContinuous(sfm::Gas::Air).ok();
    default:                                       // BusWrite, NoData, Crc, NotInitialized
      std::printf("start failed: %s\n", sfm::ToString(r.error).data());
      return false;
  }
  // --8<-- [end:errors]
}

bool sfx6000(Flow& flow) {
  // --8<-- [start:sfx6000]
  if (flow.PartFamily() == sfm::Family::Sfx6000) {
    const auto t = flow.ReadTemperature();         // output-buffer pointer, while measuring
    if (!t) {
      return false;
    }
    use_flow(t.value);
  }
  // --8<-- [end:sfx6000]
  return true;
}
