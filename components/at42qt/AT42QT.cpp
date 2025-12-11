#include "AT42QT.h"
#include "esphome/core/log.h"

namespace esphome {
namespace at42qt {

void AT42QTHub::setup(){
    ESP_LOGV(TAG, "resetting chip");
    uint8_t nzv=1;
    this->write_register(this->chip_spec->register_map->at(RESET), &nzv, 1);

    this->set_timeout(250, [this]() {//chip reset after approx 200ms
        uint8_t chip_id = 0;
        this->read_register(this->chip_spec->register_map->at(CHIP_ID), &chip_id, 1);
        ESP_LOGD(TAG, "chip ID is 0x%02x.", chip_id);
        if (chip_id != this->chip_spec->chip_id) this->mark_failed(MSG_CHIP_ID_INCORRECT);
        
        //write inital parameters to chip
        ESP_LOGV(TAG, "setting registers");
        this->set_charge_time(this->charge_time);
        this->set_toward_touch_drift(this->toward_touch_drift);
        this->set_away_touch_drift(this->away_touch_drift);
        this->set_detection_integrator(this->detection_integrator);
        this->set_touch_recal_delay(this->touch_recal_delay);
        this->set_drift_hold_time(this->drift_hold_time);  
        for(auto *chan : this->touch_channels_) {
            //go through all channels, read their config, write it to chip
            this->set_threshold(chan->get_channel(), chan->get_threshold());
            this->set_oversampling(chan->get_channel(), chan->get_oversampling());
        }

        uint8_t nzv=1;
        this->write_register(this->chip_spec->register_map->at(CALIBRATE), &nzv, 1);
        ESP_LOGV(TAG, "calibration started");
        this->finished_setup=true;
    });
}

void AT42QTHub::loop(){
    if (!this->finished_setup) return;

    //read status
    union {
        uint32_t i;
        uint8_t b[4]; 
    } chip_reg;
    this->read_register(this->chip_spec->register_map->at(STATUS), &(chip_reg.b[0]), 4);
    AT42QTStatus status = parse_status(chip_reg.i);

    //check that cal has ended
    static uint8_t cal_status = 255;
    if (status.calibrating != cal_status) {
        ESP_LOGD(TAG, "calibration active: %d", status.calibrating);
        cal_status=status.calibrating;
    }
    if (status.calibrating) return;

    //send keystate to binsensors
    for(auto *chan : this->touch_channels_) {
        chan->process(status.keys);
    }
    
    //update debug sensors on demand
    for(auto *sensor : this->debug_sensors_)
        if (sensor->get_wants_update()) {
            uint8_t chan = sensor->get_channel();
            uint8_t signal = 0;
            this->read_register(this->chip_spec->register_map->at(KEY_SIGNAL)+chan, &signal, 1);
            uint8_t reference = 0;
            this->read_register(this->chip_spec->register_map->at(KEY_REFERENCE)+chan, &reference, 1);
            sensor->process(signal, reference);
        }
}


AT42QTStatus AT42QTHub::parse_status(uint32_t status) const {
  AT42QTStatus ret;
  ret.any_key_touched = status & (1 << this->chip_spec->status_bitmap->any_key_touched);
  ret.overflow = status & (1 << this->chip_spec->status_bitmap->overflow);
  ret.calibrating = status & (1 << this->chip_spec->status_bitmap->calibrating);
  ret.keys = (status >> this->chip_spec->status_bitmap->keys_start) & (0xFFFFFFFF << this->chip_spec->keycount);
  return ret; //TODO maybe use unique_pointer?
}

void AT42QTHub::dump_config() const{
    ESP_LOGCONFIG(TAG,
        "Touch-Hub:\n"
        "  Pulse Length: %d\n"
        "  Toward Touch-Drift: %d\n"
        "  Away Touch-Drift: %d\n"
        "  Deglitch: %d\n"
        "  Touch Recal Delay: %d\n"
        "  Drift Hold Time: %d",
        this->charge_time,
        this-> toward_touch_drift,
        this-> away_touch_drift,
        this-> detection_integrator,
        this-> touch_recal_delay,
        this-> drift_hold_time
    );
    LOG_I2C_DEVICE(this);
    for(auto *chan : this->touch_channels_)
        chan->dump_config(); //channels are not Compnents, thus the hub has to call this manually
}

void AT42QTHub::set_threshold(uint8_t channel, uint8_t threshold) {
    this->write_register(this->chip_spec->register_map->at(KEY_DETECT_THRESHOLD) + channel, &threshold, 1);
    ESP_LOGD(TAG, "Set channel %d threshold to %d.", channel, threshold);
}
void AT42QTHub::set_oversampling(uint8_t channel, uint8_t oversampling) {
    this->write_register(this->chip_spec->register_map->at(KEY_PULSE_SCALE) + channel, &oversampling, 1);
    ESP_LOGD(TAG, "Set channel %d oversampling to 0x%02x.", channel, oversampling);
}

void AT42QTHub::set_charge_time(uint8_t charge_time) {
    this->write_register(this->chip_spec->register_map->at(CHARGE_DURATION), &charge_time, 1);
    this->charge_time=charge_time;
    ESP_LOGD(TAG, "Set charge_time to %d.", charge_time);
}
void AT42QTHub::set_toward_touch_drift(uint8_t toward_touch_drift) {
    this->write_register(this->chip_spec->register_map->at(TOWARDS_DRIFT_COMPENSATION_DURATION), &toward_touch_drift, 1);
    this->toward_touch_drift=toward_touch_drift;
    ESP_LOGD(TAG, "Set toward_touch_drift to %d.", toward_touch_drift);
}
void AT42QTHub::set_away_touch_drift(uint8_t away_touch_drift) {
    this->write_register(this->chip_spec->register_map->at(AWAY_DRIFT_COMPENSATION_DURATION), &away_touch_drift, 1);
    this->away_touch_drift=away_touch_drift;
    ESP_LOGD(TAG, "Set away_touch_drift to %d.", away_touch_drift);
}
void AT42QTHub::set_detection_integrator(uint8_t detection_integrator) {
    this->write_register(this->chip_spec->register_map->at(DETECTION_INTEGRATOR), &detection_integrator, 1);
    this->detection_integrator=detection_integrator;
    ESP_LOGD(TAG, "Set detection_integrator to %d.", detection_integrator);
}
void AT42QTHub::set_touch_recal_delay(uint8_t touch_recal_delay) {
    this->write_register(this->chip_spec->register_map->at(RECALIBRATION_DELAY), &touch_recal_delay, 1);
    this->touch_recal_delay=touch_recal_delay;
    ESP_LOGD(TAG, "Set charge_time to %d.", touch_recal_delay);
}
void AT42QTHub::set_drift_hold_time(uint8_t drift_hold_time) {
    this->write_register(this->chip_spec->register_map->at(DRIFT_COMPENSATION_HOLD_DURATION), &drift_hold_time, 1);
    this->drift_hold_time=drift_hold_time;
    ESP_LOGD(TAG, "Set drift_hold_time to %d.", drift_hold_time);
}


void AT42QTChannel::process(uint16_t keys) {
    this->publish_state(static_cast<bool>(keys & (1 << this->channel)));
}

uint8_t AT42QTChannel::get_channel() const {return this->channel;};
uint8_t AT42QTChannel::get_threshold() const {return this->threshold;};
uint8_t AT42QTChannel::get_oversampling() const {return this->oversampling;};

void AT42QTChannel::dump_config() const {
    LOG_BINARY_SENSOR(TAG, " Touch Key", this);
    ESP_LOGCONFIG(TAG,
        " Channel: %d\n"
        " Threshold: %d\n"
        " Oversampling: 0x%02x",
        this->channel,
        this->threshold,
        this->oversampling
    );
}



uint8_t AT42QTDebug::get_channel() const {return this->channel;};
bool AT42QTDebug::get_wants_update() const {return this->wants_update;};
void AT42QTDebug::update() override {this->wants_update=true;};

void AT42QTDebug::process(uint8_t signal, uint8_t reference) {
    if (this->sensor_sig != nullptr) this->sensor_sig->publish_state(signal);
    if (this->sensor_sig != nullptr) this->sensor_ref->publish_state(reference);
    this->wants_update=false;
}

void AT42QTDebug::dump_config() const {
    ESP_LOGCONFIG(TAG,
        "Debug Sensor\n"
        "  Channel: %d",
        this->channel
    );
    //TODO: nicer output here, indent sensor log
    if (this->sensor_sig != nullptr) LOG_SENSOR(TAG, " signal", sensor_sig);
    if (this->sensor_sig != nullptr) LOG_SENSOR(TAG, " reference", sensor_ref);
}

} //namespace at42qt
} //namespace esphome