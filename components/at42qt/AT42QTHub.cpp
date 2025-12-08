#include "AT42QTHub.h"
#include "esphome/core/log.h"

namespace esphome {
namespace at42qt {

void AT42QTHub::setup(){
    ESP_LOGD(TAG, "setting up...");
    uint8_t nzv=1;
    this->write_register((uint8_t)RESET, &nzv, 1);

    this->set_timeout(250, [this]() {//chip reset after approx 200ms
        uint8_t chip_id = 0;
        this->read_register((uint8_t)CHIP_ID, &chip_id, 1);
        ESP_LOGD(TAG, "Chip ID is %02x.", chip_id);
        if (chip_id != 0x3E) this->mark_failed(i2c_fail_msg);
        
        //set parameters
        this->set_charge_time(this->charge_time);
        this->set_toward_touch_drift(this->toward_touch_drift);
        this->set_away_touch_drift(this->away_touch_drift);
        this->set_detection_integrator(this->detection_integrator);
        this->set_touch_recal_delay(this->touch_recal_delay);
        this->set_drift_hold_time(this->drift_hold_time);  
        for(auto *chan : this->binary_sensors_) {
            this->set_threshold(chan->get_channel(), chan->get_threshold());
            this->set_oversampling(chan->get_channel(), chan->get_oversampling());
        }

        uint8_t nzv=1;
        this->write_register((uint8_t)CALIBRATE, &nzv, 1);
        ESP_LOGD(TAG, "calibration started...");
        this->finished_setup=true;
    });
}

void AT42QTHub::loop(){
    if (!this->finished_setup) return;
    static uint8_t cal_status = 255;
    AT42QT2120_Status status; 
    this->read_register((uint8_t)STATUS, &(status.bytes[0]), 4);
    if (status.calibrating != cal_status) {
        ESP_LOGD(TAG, "calibration active: %d", status.calibrating);
        cal_status=status.calibrating;
    }
    if (status.calibrating) return;

    for(auto *binary_sensor : this->binary_sensors_) binary_sensor->process(status.keys); //send keystate to binsensors
    
    for(auto *sensor : this->sensors_) //update debug sensors on demand
        if (sensor->get_wants_update()) {
            uint8_t chan = sensor->get_channel();
            uint8_t signal = 0;
            this->read_register((uint8_t)KEY_SIGNAL+chan, &signal, 1);
            uint8_t reference = 0;
            this->read_register((uint8_t)KEY_REFERENCE+chan, &reference, 1);
            sensor->process(signal, reference);
        }
}

void AT42QTHub::dump_config(){
    LOG_I2C_DEVICE(this);
    for(auto *chan : this->binary_sensors_)
        chan->dump_config();
    ESP_LOGD(TAG, "Pulse Length: %d", this->charge_time);
    ESP_LOGD(TAG, "Toward Touch-Drift: %d", this-> toward_touch_drift);
    ESP_LOGD(TAG, "Away Touch-Drift: %d", this-> away_touch_drift);
    ESP_LOGD(TAG, "detection_integrator: %d", this-> detection_integrator);
    ESP_LOGD(TAG, "Touch Recal Delay: %d", this-> touch_recal_delay);
    ESP_LOGD(TAG, "Drift Hold Time: %d", this-> drift_hold_time); 
}

void AT42QTHub::set_threshold(uint8_t channel, uint8_t threshold) {
    this->write_register((uint8_t)KEY_DETECT_THRESHOLD + channel, &threshold, 1);
    ESP_LOGD(TAG, "Set channel %d threshold to %d.", channel, threshold);
}
void AT42QTHub::set_oversampling(uint8_t channel, uint8_t oversampling) {
    this->write_register((uint8_t)KEY_PULSE_SCALE + channel, &oversampling, 1);
    ESP_LOGD(TAG, "Set channel %d oversampling to %02x.", channel, oversampling);
}

void AT42QTHub::set_charge_time(uint8_t charge_time) {
    this->write_register((uint8_t)CHARGE_DURATION, &charge_time, 1);
    this->charge_time=charge_time;
    ESP_LOGD(TAG, "Set charge_time to %d.", charge_time);
}
void AT42QTHub::set_toward_touch_drift(uint8_t toward_touch_drift) {
    this->write_register((uint8_t)TOWARDS_DRIFT_COMPENSATION_DURATION, &toward_touch_drift, 1);
    this->toward_touch_drift=toward_touch_drift;
    ESP_LOGD(TAG, "Set toward_touch_drift to %d.", toward_touch_drift);
}
void AT42QTHub::set_away_touch_drift(uint8_t away_touch_drift) {
    this->write_register((uint8_t)AWAY_DRIFT_COMPENSATION_DURATION, &away_touch_drift, 1);
    this->away_touch_drift=away_touch_drift;
    ESP_LOGD(TAG, "Set away_touch_drift to %d.", away_touch_drift);
}
void AT42QTHub::set_detection_integrator(uint8_t detection_integrator) {
    this->write_register((uint8_t)DETECTION_INTEGRATOR, &detection_integrator, 1);
    this->detection_integrator=detection_integrator;
    ESP_LOGD(TAG, "Set detection_integrator to %d.", detection_integrator);
}
void AT42QTHub::set_touch_recal_delay(uint8_t touch_recal_delay) {
    this->write_register((uint8_t)RECALIBRATION_DELAY, &touch_recal_delay, 1);
    this->touch_recal_delay=touch_recal_delay;
    ESP_LOGD(TAG, "Set charge_time to %d.", touch_recal_delay);
}
void AT42QTHub::set_drift_hold_time(uint8_t drift_hold_time) {
    this->write_register((uint8_t)DRIFT_COMPENSATION_HOLD_DURATION, &drift_hold_time, 1);
    this->drift_hold_time=drift_hold_time;
    ESP_LOGD(TAG, "Set drift_hold_time to %d.", drift_hold_time);
}


uint8_t AT42QTChannel::get_channel() const {return this->channel;};
uint8_t AT42QTChannel::get_threshold() const {return this->threshold;};
uint8_t AT42QTChannel::get_oversampling() const {return this->oversampling;};

void AT42QTChannel::dump_config(){
    LOG_BINARY_SENSOR(TAG, "touch sensor", this);
    ESP_LOGD(TAG, "Channel: %d", this->channel);
    ESP_LOGD(TAG, "Threshold: %d", this->threshold);
    ESP_LOGD(TAG, "Oversampling: %02x", this->oversampling);
}



uint8_t AT42QTDebug::get_channel() const {return this->channel;};
bool AT42QTDebug::get_wants_update() const {return this->wants_update;};

void AT42QTDebug::process(uint8_t signal, uint8_t reference) {
    if (this->sensor_sig != nullptr) this->sensor_sig->publish_state(signal);
    if (this->sensor_sig != nullptr) this->sensor_ref->publish_state(reference);
    this->wants_update=false;
}

void AT42QTDebug::dump_config(){
    if (this->sensor_sig != nullptr) LOG_SENSOR(TAG, "signal sensor", sensor_sig);
    if (this->sensor_sig != nullptr) LOG_SENSOR(TAG, "reference sensor", sensor_ref);
    ESP_LOGD(TAG, "Channel: %d", this->channel);
}

} //namespace at42qt
} //namespace esphome