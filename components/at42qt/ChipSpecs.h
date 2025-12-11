/*
 * Register-maps and specifications for all chips of the family.
 */
#pragma once
#include <map>
#include <cstdint>

namespace esphome {
namespace at42qt {

// -- Register Maps -- //
enum AT42QTRegister : uint8_t { //enum of all possible registers, to be used in chip-specific std::map
    ACTIVE_LEVEL_MASK,
    ADJACENT_KEY_SUPRESSION_MASK,
    AWAY_DRIFT_COMPENSATION_DURATION,
    BURST_REPETITION,
    CALIBRATE,
    CHARGE_DURATION,
    CHIP_ID,
    COMMON_CHANGE_KEYS,
    DETECTION_INTEGRATOR,
    DETECTION_MASK,
    DRIFT_COMPENSATION_HOLD_DURATION,
    DRIFT_OPTION,
    FO_MAX_CAL_GUARD_CHANNEL,
    GPIO_DIRECTION,
    GPIO_GPO_DRIVE,
    GPIO_GPO_PWM,
    GPIO_WAKE,
    IO_MASK,
    KEY_AKS_LEVEL,
    KEY_BURST_LENGTH,
    KEY_CONTROL,
    KEY_DETECT_INTEGRATOR,
    KEY_DETECT_THRESHOLD,
    KEY_MASK,
    KEY_PULSE_SCALE,
    KEY_REFERENCE,
    KEY_SIGNAL,
    LOW_POWER_MODE,
    MAX_ON_DURATION,
    PWM_LEVEL,
    PWM_MASK,
    RECALIBRATION_DELAY,
    RESET,
    SLIDER_CONTROL,
    SLIDER_OPTIONS,
    SLIDER_OR_WHEEL_ENABLE,
    STATUS,
    SUB_REVISION,
    TOWARDS_DRIFT_COMPENSATION_DURATION,
    USER_OUTPUT_BUFFER,
    VERSION
};

typedef std::map<AT42QTRegister, uint8_t> AT42QTRegisterMap;

// -- status-parsing -- //
struct AT42QTStatus {
    bool any_key_touched;
    bool overflow;
    bool calibrating;
    uint16_t keys;
};

struct AT42QTStatusBitmap { //answers: at which bit in the status is x located? 255 means invalid
    const uint8_t any_key_touched;
    const uint8_t overflow;
    const uint8_t calibrating;
    const uint8_t keys_start;
};

struct AT42QTSpec {
    const uint8_t keycount;
    const uint8_t chip_id;
    const uint8_t i2c_addr;
    const AT42QTRegisterMap *register_map;
    const AT42QTStatusBitmap *status_bitmap;
};

//from AT42QT.cpp
extern std::map<uint16_t, const AT42QTSpec*> chipnum_to_spec;

} //namespace at42qt
} //namespace esphome
