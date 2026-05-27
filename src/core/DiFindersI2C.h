#pragma once

#include <Arduino.h>
#include <Wire.h>

namespace DiFinders {
namespace detail {

static constexpr uint8_t kI2cRetryCount = 3;

inline void i2cRetryBackoff() {
  delayMicroseconds(40);
}

inline void discardPendingWireBytes(TwoWire* bus) {
  while (bus->available() > 0) {
    bus->read();
  }
}

inline bool writeReg8(TwoWire* bus, uint8_t address, uint8_t reg, uint8_t value, uint8_t* lastStatus = nullptr) {
  uint8_t status = 4;
  for (uint8_t attempt = 0; attempt < kI2cRetryCount; ++attempt) {
    bus->beginTransmission(address);
    bus->write(reg);
    bus->write(value);
    status = bus->endTransmission();
    if (status == 0) {
      break;
    }
    i2cRetryBackoff();
  }
  if (lastStatus != nullptr) {
    *lastStatus = status;
  }
  return status == 0;
}

inline bool writeReg16(TwoWire* bus, uint8_t address, uint8_t reg, uint16_t value, uint8_t* lastStatus = nullptr) {
  uint8_t status = 4;
  for (uint8_t attempt = 0; attempt < kI2cRetryCount; ++attempt) {
    bus->beginTransmission(address);
    bus->write(reg);
    bus->write(static_cast<uint8_t>(value >> 8));
    bus->write(static_cast<uint8_t>(value));
    status = bus->endTransmission();
    if (status == 0) {
      break;
    }
    i2cRetryBackoff();
  }
  if (lastStatus != nullptr) {
    *lastStatus = status;
  }
  return status == 0;
}

inline bool writeReg32(TwoWire* bus, uint8_t address, uint8_t reg, uint32_t value, uint8_t* lastStatus = nullptr) {
  uint8_t status = 4;
  for (uint8_t attempt = 0; attempt < kI2cRetryCount; ++attempt) {
    bus->beginTransmission(address);
    bus->write(reg);
    bus->write(static_cast<uint8_t>(value >> 24));
    bus->write(static_cast<uint8_t>(value >> 16));
    bus->write(static_cast<uint8_t>(value >> 8));
    bus->write(static_cast<uint8_t>(value));
    status = bus->endTransmission();
    if (status == 0) {
      break;
    }
    i2cRetryBackoff();
  }
  if (lastStatus != nullptr) {
    *lastStatus = status;
  }
  return status == 0;
}

inline bool readReg8(TwoWire* bus, uint8_t address, uint8_t reg, uint8_t& value, uint8_t* lastStatus = nullptr) {
  uint8_t status = 4;
  for (uint8_t attempt = 0; attempt < kI2cRetryCount; ++attempt) {
    bus->beginTransmission(address);
    bus->write(reg);
    status = bus->endTransmission(false);
    if (status == 0 && bus->requestFrom(address, static_cast<uint8_t>(1)) == 1) {
      value = bus->read();
      if (lastStatus != nullptr) {
        *lastStatus = 0;
      }
      return true;
    }
    discardPendingWireBytes(bus);
    if (status == 0) {
      status = 4;
    }
    i2cRetryBackoff();
  }
  if (lastStatus != nullptr) {
    *lastStatus = status;
  }
  return false;
}

inline bool readReg16(TwoWire* bus, uint8_t address, uint8_t reg, uint16_t& value, uint8_t* lastStatus = nullptr) {
  uint8_t status = 4;
  for (uint8_t attempt = 0; attempt < kI2cRetryCount; ++attempt) {
    bus->beginTransmission(address);
    bus->write(reg);
    status = bus->endTransmission(false);
    if (status == 0 && bus->requestFrom(address, static_cast<uint8_t>(2)) == 2) {
      value = static_cast<uint16_t>(bus->read()) << 8;
      value |= static_cast<uint16_t>(bus->read());
      if (lastStatus != nullptr) {
        *lastStatus = 0;
      }
      return true;
    }
    discardPendingWireBytes(bus);
    if (status == 0) {
      status = 4;
    }
    i2cRetryBackoff();
  }
  if (lastStatus != nullptr) {
    *lastStatus = status;
  }
  return false;
}

inline bool readReg32(TwoWire* bus, uint8_t address, uint8_t reg, uint32_t& value, uint8_t* lastStatus = nullptr) {
  uint8_t status = 4;
  for (uint8_t attempt = 0; attempt < kI2cRetryCount; ++attempt) {
    bus->beginTransmission(address);
    bus->write(reg);
    status = bus->endTransmission(false);
    if (status == 0 && bus->requestFrom(address, static_cast<uint8_t>(4)) == 4) {
      value = static_cast<uint32_t>(bus->read()) << 24;
      value |= static_cast<uint32_t>(bus->read()) << 16;
      value |= static_cast<uint32_t>(bus->read()) << 8;
      value |= static_cast<uint32_t>(bus->read());
      if (lastStatus != nullptr) {
        *lastStatus = 0;
      }
      return true;
    }
    discardPendingWireBytes(bus);
    if (status == 0) {
      status = 4;
    }
    i2cRetryBackoff();
  }
  if (lastStatus != nullptr) {
    *lastStatus = status;
  }
  return false;
}

inline bool writeReg8_16(TwoWire* bus, uint8_t address, uint16_t reg, uint8_t value, uint8_t* lastStatus = nullptr) {
  uint8_t status = 4;
  for (uint8_t attempt = 0; attempt < kI2cRetryCount; ++attempt) {
    bus->beginTransmission(address);
    bus->write(static_cast<uint8_t>(reg >> 8));
    bus->write(static_cast<uint8_t>(reg));
    bus->write(value);
    status = bus->endTransmission();
    if (status == 0) {
      break;
    }
    i2cRetryBackoff();
  }
  if (lastStatus != nullptr) {
    *lastStatus = status;
  }
  return status == 0;
}

inline bool writeReg16_16(TwoWire* bus, uint8_t address, uint16_t reg, uint16_t value, uint8_t* lastStatus = nullptr) {
  uint8_t status = 4;
  for (uint8_t attempt = 0; attempt < kI2cRetryCount; ++attempt) {
    bus->beginTransmission(address);
    bus->write(static_cast<uint8_t>(reg >> 8));
    bus->write(static_cast<uint8_t>(reg));
    bus->write(static_cast<uint8_t>(value >> 8));
    bus->write(static_cast<uint8_t>(value));
    status = bus->endTransmission();
    if (status == 0) {
      break;
    }
    i2cRetryBackoff();
  }
  if (lastStatus != nullptr) {
    *lastStatus = status;
  }
  return status == 0;
}

inline bool readReg8_16(TwoWire* bus, uint8_t address, uint16_t reg, uint8_t& value, uint8_t* lastStatus = nullptr) {
  uint8_t status = 4;
  for (uint8_t attempt = 0; attempt < kI2cRetryCount; ++attempt) {
    bus->beginTransmission(address);
    bus->write(static_cast<uint8_t>(reg >> 8));
    bus->write(static_cast<uint8_t>(reg));
    status = bus->endTransmission(false);
    if (status == 0 && bus->requestFrom(address, static_cast<uint8_t>(1)) == 1) {
      value = bus->read();
      if (lastStatus != nullptr) {
        *lastStatus = 0;
      }
      return true;
    }
    discardPendingWireBytes(bus);
    if (status == 0) {
      status = 4;
    }
    i2cRetryBackoff();
  }
  if (lastStatus != nullptr) {
    *lastStatus = status;
  }
  return false;
}

inline bool readReg16_16(TwoWire* bus, uint8_t address, uint16_t reg, uint16_t& value, uint8_t* lastStatus = nullptr) {
  uint8_t status = 4;
  for (uint8_t attempt = 0; attempt < kI2cRetryCount; ++attempt) {
    bus->beginTransmission(address);
    bus->write(static_cast<uint8_t>(reg >> 8));
    bus->write(static_cast<uint8_t>(reg));
    status = bus->endTransmission(false);
    if (status == 0 && bus->requestFrom(address, static_cast<uint8_t>(2)) == 2) {
      value = static_cast<uint16_t>(bus->read()) << 8;
      value |= static_cast<uint16_t>(bus->read());
      if (lastStatus != nullptr) {
        *lastStatus = 0;
      }
      return true;
    }
    discardPendingWireBytes(bus);
    if (status == 0) {
      status = 4;
    }
    i2cRetryBackoff();
  }
  if (lastStatus != nullptr) {
    *lastStatus = status;
  }
  return false;
}

inline bool readReg32_16(TwoWire* bus, uint8_t address, uint16_t reg, uint32_t& value, uint8_t* lastStatus = nullptr) {
  uint8_t status = 4;
  for (uint8_t attempt = 0; attempt < kI2cRetryCount; ++attempt) {
    bus->beginTransmission(address);
    bus->write(static_cast<uint8_t>(reg >> 8));
    bus->write(static_cast<uint8_t>(reg));
    status = bus->endTransmission(false);
    if (status == 0 && bus->requestFrom(address, static_cast<uint8_t>(4)) == 4) {
      value = static_cast<uint32_t>(bus->read()) << 24;
      value |= static_cast<uint32_t>(bus->read()) << 16;
      value |= static_cast<uint32_t>(bus->read()) << 8;
      value |= static_cast<uint32_t>(bus->read());
      if (lastStatus != nullptr) {
        *lastStatus = 0;
      }
      return true;
    }
    discardPendingWireBytes(bus);
    if (status == 0) {
      status = 4;
    }
    i2cRetryBackoff();
  }
  if (lastStatus != nullptr) {
    *lastStatus = status;
  }
  return false;
}

}  // namespace detail
}  // namespace DiFinders
