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
        this->set_pulse_length(this->pulse_length);
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
    ESP_LOGD(TAG, "Pulse length: %d", this->pulse_length);
}

void AT42QTHub::set_threshold(uint8_t channel, uint8_t threshold) {
    this->write_register((uint8_t)KEY_DETECT_THRESHOLD + channel, &threshold, 1);
    ESP_LOGD(TAG, "Set channel %d threshold to %d.", channel, threshold);
}
void AT42QTHub::set_oversampling(uint8_t channel, uint8_t oversampling) {
    this->write_register((uint8_t)KEY_PULSE_SCALE + channel, &oversampling, 1);
    ESP_LOGD(TAG, "Set channel %d oversampling to %02x.", channel, oversampling);
}

void AT42QTHub::set_pulse_length(uint8_t pulse_length) {
    this->write_register((uint8_t)CHARGE_DURATION, &pulse_length, 1);
    this->pulse_length=pulse_length;
    ESP_LOGD(TAG, "Set pulse_length to %d.", pulse_length);
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