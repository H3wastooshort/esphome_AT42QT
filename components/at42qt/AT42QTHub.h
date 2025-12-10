#pragma once

#include "esphome/core/component.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/sensor/sensor.h"

#include "ChipSpecs.h"

namespace esphome {
namespace at42qt {
static const char *const TAG = "at42qt";

static const char *const i2c_fail_msg = "Read invalid chip ID. Check part number and wiring.";

class AT42QTHub;

class AT42QTDebug : public PollingComponent {
 public:
  AT42QTDebug(uint8_t channel) : channel(channel) {};
  void process(uint8_t signal, uint8_t reference);
  uint8_t get_channel() const;
  bool get_wants_update() const;

  void set_sensor_sig(sensor::Sensor *sensor_sig) { this->sensor_sig = sensor_sig; }
  void set_sensor_ref(sensor::Sensor *sensor_ref) { this->sensor_ref = sensor_ref; }

  void setup() override {};
  void loop() override {};
  void update() override {this->wants_update=true;};
  void dump_config() override;

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
  void process(uint16_t keys);

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
  AT42QTHub(const uint16_t chip_num, const uint8_t charge_time, const uint8_t toward_touch_drift, const uint8_t away_touch_drift, const uint8_t detection_integrator, const uint8_t touch_recal_delay, const uint8_t drift_hold_time) : chip_spec(chipnum_to_spec.at(chip_num)), charge_time(charge_time), toward_touch_drift(toward_touch_drift), away_touch_drift(away_touch_drift), detection_integrator(detection_integrator), touch_recal_delay(touch_recal_delay), drift_hold_time(drift_hold_time) {}
  AT42QTHub(const AT42QTSpec* chip_spec, const uint8_t charge_time, const uint8_t toward_touch_drift, const uint8_t away_touch_drift, const uint8_t detection_integrator, const uint8_t touch_recal_delay, const uint8_t drift_hold_time) : chip_spec(chip_spec), charge_time(charge_time), toward_touch_drift(toward_touch_drift), away_touch_drift(away_touch_drift), detection_integrator(detection_integrator), touch_recal_delay(touch_recal_delay), drift_hold_time(drift_hold_time) {}
  void register_channel(AT42QTChannel *obj) { this->binary_sensors_.push_back(obj); }
  void register_debug(AT42QTDebug *obj) { this->sensors_.push_back(obj); }
  void setup() override;
  void loop() override;
  void dump_config() override;

  void set_threshold(uint8_t channel, uint8_t threshold);
  void set_oversampling(uint8_t channel, uint8_t oversampling);
  //names from datasheet at https://ww1.microchip.com/downloads/en/DeviceDoc/doc9634.pdf
  void set_charge_time(uint8_t charge_time);
  void set_toward_touch_drift(uint8_t toward_touch_drift);
  void set_away_touch_drift(uint8_t away_touch_drift);
  void set_detection_integrator(uint8_t detection_integrator);
  void set_touch_recal_delay(uint8_t touch_recal_delay);
  void set_drift_hold_time(uint8_t drift_hold_time);  
  
 protected:
  AT42QTStatus parse_status(uint32_t status);

 private:
  std::vector<AT42QTChannel *> binary_sensors_;
  std::vector<AT42QTDebug *> sensors_;
  bool finished_setup{false};

  const AT42QTSpec* chip_spec;
  uint8_t charge_time;
  uint8_t toward_touch_drift;
  uint8_t away_touch_drift;
  uint8_t detection_integrator;
  uint8_t touch_recal_delay;
  uint8_t drift_hold_time;
};

} //namespace at42qt
} //namespace esphome