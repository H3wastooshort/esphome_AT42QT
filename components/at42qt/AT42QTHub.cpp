#include "AT42QTHub.h"
#include "esphome/core/log.h"

namespace esphome {
namespace at42qt {

void AT42QTHub::setup(){

    uint8_t nzv=1;
    this->write_register((uint8_t)RESET, &nzv, 1);

    uint8_t chip_id = 0;
    this->read_register((uint8_t)CHIP_ID, &chip_id, 1);
    if (chip_id != 0x3E) this->mark_failed(i2c_fail_msg);
            
    this->write_register((uint8_t)CALIBRATE, &nzv, 1);
}

void AT42QTHub::loop(){
    //TODO: check chip_id==0x3E
    AT42QT2120_Status status; 
    this->read_register((uint8_t)STATUS, &(status.bytes[0]), 4);
    if (status.calibrating) return;
    for(auto *binary_sensor : this->binary_sensors_) binary_sensor->process(status.keys); //send keystate to binsensors
}

void AT42QTHub::dump_config(){
    LOG_I2C_DEVICE(this);
    for(auto *binary_sensor : this->binary_sensors_){
        LOG_BINARY_SENSOR(TAG, "Binary sensor", binary_sensor);
    }
}

void AT42QTHub::set_threshold(uint8_t channel, uint8_t threshold) {
    this->write_register((uint8_t)KEY_DETECT_THRESHOLD + channel, &threshold, 1);
}
void AT42QTHub::set_oversampling(uint8_t channel, uint8_t oversampling) {
    this->write_register((uint8_t)KEY_PULSE_SCALE + channel, &oversampling, 1);
}

void AT42QTHub::set_pulse_length(uint8_t pulse_length) {
    this->write_register((uint8_t)CHARGE_DURATION, &pulse_length, 1);
}

} //namespace at42qt
} //namespace esphome