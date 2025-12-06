#pragma once

#include "esphome/core/component.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/components/binary_sensor/binary_sensor.h"


namespace esphome {
namespace at42qt {
static const char *const TAG = "at42qt.component";

static const char *const i2c_fail_msg = "Read invalid chip ID. Check part number and wiring.";

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

class AT42QTHub;

class AT42QTDebug : public sensor::Sensor, public PollingComponent {
 public:
  AT42QTDebug(uint8_t channel) : channel(channel) {};
  void dump_config();
  void process(uint8_t signal, uint8_t reference);
  uint8_t get_channel() const;
  bool get_wants_update() const;

  void set_signal(sensor::Sensor *sensor_sig) { this->sensor_sig = sensor_sig; }
  void set_reference(sensor::Sensor *sensor_ref) { this->sensor_ref = sensor_ref; }

  void update() override {this->wants_update=true;};
  //void dump_config() override;

 protected:
  bool wants_update{true};
  uint8_t channel{0};
  sensor::Sensor *sensor_sig{nullptr};
  sensor::Sensor *sensor_ref{nullptr};
};

class AT42QTChannel : public binary_sensor::BinarySensor {
 public:
  AT42QTChannel(uint8_t channel, uint8_t threshold, uint8_t oversampling) : channel(channel), threshold(threshold), oversampling(oversampling) {};
  void dump_config();
  void process(uint32_t data) { this->publish_state(static_cast<bool>(data & (1 << this->channel))); }

  uint8_t get_channel() const;
  uint8_t get_threshold() const;
  uint8_t get_oversampling() const;
 protected:
  uint8_t channel{0};
  uint8_t threshold{10};
  uint8_t oversampling{0};
};

class AT42QTHub : public Component, public i2c::I2CDevice {
 public:
  AT42QTHub(uint8_t pulse_length) : pulse_length(pulse_length) {}
  void register_channel(AT42QTChannel *obj) { this->binary_sensors_.push_back(obj); }
  void register_debug(AT42QTDebug *obj) { this->sensors_.push_back(obj); }
  void setup() override;
  void loop() override;
  void dump_config() override;

  void set_threshold(uint8_t channel, uint8_t threshold);
  void set_oversampling(uint8_t channel, uint8_t oversampling);

  void set_pulse_length(uint8_t pulse_length);
  
 protected:
  std::vector<AT42QTChannel *> binary_sensors_;
  std::vector<AT42QTDebug *> sensors_;
  uint8_t pulse_length{0};
  bool finished_setup{false};
};

} //namespace at42qt
} //namespace esphome