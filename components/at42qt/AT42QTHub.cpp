#include "AT42QTHub.h"
#include "esphome/core/log.h"

namespace esphome {
namespace at42qt {

static const char *TAG = "at42qt.component";

void AT42QTHub::setup(){
    //TODO: check chip_id==0x3E

    uint8_t nzv=1;
    this->write_register((uint8_t)RESET, &nzv, 1);
    
    uint8_t high_sensitivity = 0x84;
    for (uint8_t r = (uint8_t)KEY_PULSE_SCALE; r<(uint8_t)KEY_CONTROL; i++)
        this->write_register(r, &high_sensitivity, 1);
    
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
        LOG_BINARY_SENSOR("  ", "Binary sensor", binary_sensor);
    }
}

} //namespace at42qt
} //namespace esphome