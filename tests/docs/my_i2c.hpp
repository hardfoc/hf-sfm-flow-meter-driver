/**
 * @file my_i2c.hpp
 * @brief The I2C port shown on the documentation's Porting page, compiled by the host build.
 *
 * Every platform call goes through the four `platform_*` functions declared below; a real port
 * replaces them with its SDK (ESP-IDF, STM32 HAL, Zephyr, Linux i2c-dev). The class is what the
 * driver needs, nothing more.
 */
#pragma once

#include "sfm.hpp"

#include <cstddef>
#include <cstdint>

// Platform hooks: your SDK's calls. Declared only, so the docs build checks the port compiles.
bool platform_i2c_init() noexcept;
bool platform_i2c_write(uint8_t addr7, const uint8_t* data, std::size_t len) noexcept;  // START .. STOP
bool platform_i2c_read(uint8_t addr7, uint8_t* out, std::size_t len) noexcept;          // START .. STOP
void platform_delay_ms(uint32_t ms) noexcept;

// --8<-- [start:port]
class MyI2c : public sfm::I2cInterface<MyI2c> {
public:
  // Raw write: START, address + W, the bytes, STOP. No register byte. len == 0 is an
  // address-only probe (ExitSleep() uses it). Return false on a NACK or bus error.
  bool Write(uint8_t addr7, const uint8_t* data, std::size_t len) noexcept {
    return platform_i2c_write(addr7, data, len);
  }

  // Raw read: START, address + R, len bytes, STOP. Return false on a NACK: the sensor
  // NACKs its address while it has no new sample, and the driver reports NoData.
  bool Read(uint8_t addr7, uint8_t* out, std::size_t len) noexcept {
    return platform_i2c_read(addr7, out, len);
  }

  // Blocking wait: start-up (12 ms), soft reset (20 or 30 ms), wake-up polling (5 ms steps).
  void DelayMs(uint32_t ms) noexcept { platform_delay_ms(ms); }

  // Bring the bus up on first use; every command checks it.
  bool EnsureInitialized() noexcept {
    if (!ready_) {
      ready_ = platform_i2c_init();
    }
    return ready_;
  }

  // Optional: one byte to the general-call address 0x00. Without it SoftReset() returns NotSupported.
  bool WriteGeneralCallImpl(uint8_t byte) noexcept {
    return platform_i2c_write(sfm::addr::kGeneralCall, &byte, 1);
  }

private:
  bool ready_ = false;
};
// --8<-- [end:port]
