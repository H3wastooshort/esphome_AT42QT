#include "AT42QTHub.h"
#include "esphome/core/log.h"

namespace esphome {
namespace at42qt {

static const char *TAG = "at42qt.component";

void AT42QTHub::setup(){
    //TODO: check chip_id==0x3E

    uint8_t nzv=1;
    this->write_register((uint8_t)RESET, &nzv, 1);
    
    uint8_t thresh = 2; //default was 10, this is very low
    for (uint8_t r = (uint8_t)KEY_DETECT_THRESHOLD; r<(uint8_t)KEY_CONTROL; r++)
        this->write_register(r, &thresh, 1);

    /*uint8_t oversample = 0x50; //oversample=4^5 scale=2^0
    for (uint8_t r = (uint8_t)KEY_PULSE_SCALE; r<(uint8_t)KEY_SIGNAL; r++)
        this->write_register(r, &oversample, 1);*/

    uint8_t charge_time = 128;
    this->write_register((uint8_t)CHARGE_DURATION, &charge_time, 1);
            
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