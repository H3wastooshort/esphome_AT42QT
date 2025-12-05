#include "esphome/core/log.h"

namespace esphome {
namespace at42qt {

static const char *TAG = "at42qt.component";

void AT42QTHub::setup(){
    uint8_t nzv=1;
    this->write_register(RESET, &nzv, 1);
    this->write_register(CALIBRATE, &nzv, 1);
    //TODO: check chip_id==0x3E
}

void AT42QTHub::loop(){
    if (!touch_sensor.communicating()) {return;}
    if (touch_sensor.calibrating()) return;
    AT42QT2120_Status status; 
    this->read_register(STATUS, &status.bytes,4);
    for(auto *binary_sensor : this->binary_sensors_) binary_sensor->process(status.keys); //send keystate to binsensors
}

void AT42QTHub::dump_config(){
    LOG_I2C_DEVICE(this);
    for(auto *binary_sensor : this->binary_sensors_){
        LOG_BINARY_SENSOR("  ", "Binary sensor", binary_sensor);
    }
}

} //namespace at42qt
} //namespace esphome