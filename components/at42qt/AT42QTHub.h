#pragma once

#include "esphome/core/component.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/components/binary_sensor/binary_sensor.h"


namespace esphome {
namespace at42qt {
static const char * i2c_fail = "can't communicate";


enum AT42QT2120_Registers
{
  CHIP_ID = 0,
  VERSION,
  STATUS,
  CALIBRATE = 6,
  RESET,
  LOW_POWER_MODE,
  TOWARDS_DRIFT_COMPENSATION_DURATION,
  AWAY_DRIFT_COMPENSATION_DURATION,
  DETECTION_INTEGRATOR,
  RECALIBRATION_DELAY,
  DRIFT_COMPENSATION_HOLD_DURATION,
  SLIDER_OR_WHEEL_ENABLE,
  CHARGE_DURATION,
  KEY_DETECT_THRESHOLD,
  KEY_CONTROL = 28,
  KEY_PULSE_SCALE = 40,
  KEY_SIGNAL = 52,
  KEY_REFERENCE = 76,
};

union AT42QT2120_Status {
  struct {
    uint32_t any_key_touched : 1;
    uint32_t slider_or_wheel : 1;
    uint32_t space0 : 4;
    uint32_t overflow : 1;
    uint32_t calibrating : 1;
    uint32_t keys: 12;
    uint32_t space1 : 4;
    uint32_t slider_or_wheel_position : 8;
  };
  uint8_t bytes[4];
};

class AT42QTChannel : public binary_sensor::BinarySensor {
 public:
  void set_channel(uint8_t channel) { channel_ = channel; }
  void process(uint32_t data) { this->publish_state(static_cast<bool>(data & (1 << this->channel_))); }

 protected:
  uint8_t channel_{0};
};

class AT42QTHub : public Component, public i2c::I2CDevice {
 public:
  void register_channel(AT42QTChannel *obj) { this->binary_sensors_.push_back(obj); }
  void setup() override;
  void loop() override;
  void dump_config() override;
  
 protected:
  std::vector<AT42QTChannel *> binary_sensors_;
};

} //namespace at42qt
} //namespace esphome