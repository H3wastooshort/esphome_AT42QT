#include <map>
#include <cstdint>
#include "ChipSpecs.h"

namespace esphome {
namespace at42qt {

namespace spec_data {
  //Register addresses from https://github.com/janelia-arduino/AT42QT/blob/09685cb123f61658bafe513609bf38cb96f55c0e/src/AT42QT/RegisterAddresses.h
  const std::map<AT42QTRegister, uint8_t> reg_AT42QT1060 {
    {CHIP_ID, 0},
    {VERSION, 1},
    {STATUS, 4},
    {CALIBRATE, 12},
    {RESET, 13},
    {DRIFT_OPTION, 14},
    {RECALIBRATION_DELAY, 15},
    {KEY_DETECT_THRESHOLD, 16},
    {LOW_POWER_MODE, 22},
    {IO_MASK, 23},
    {KEY_MASK, 24},
    {ADJACENT_KEY_SUPRESSION_MASK, 25},
    {PWM_MASK, 26},
    {DETECTION_MASK, 27},
    {ACTIVE_LEVEL_MASK, 28},
    {USER_OUTPUT_BUFFER, 29},
    {DETECTION_INTEGRATOR, 30},
    {PWM_LEVEL, 31},
    {KEY_SIGNAL, 40},
    {KEY_REFERENCE, 52},
  };
  const std::map<AT42QTRegister, uint8_t> reg_AT42QT1070 {
    {CHIP_ID, 0},
    {VERSION, 1},
    {STATUS, 2},
    {KEY_SIGNAL, 4},
    {KEY_REFERENCE, 18},
    {KEY_DETECT_THRESHOLD, 32},
    {KEY_AKS_LEVEL, 39},
    {KEY_DETECT_INTEGRATOR, 46},
    {FO_MAX_CAL_GUARD_CHANNEL, 53},
    {LOW_POWER_MODE, 54},
    {MAX_ON_DURATION, 55},
    {CALIBRATE, 56},
    {RESET, 57},
  };
  const std::map<AT42QTRegister, uint8_t> reg_AT42QT2120 {
    {CHIP_ID, 0},
    {VERSION, 1},
    {STATUS, 2},
    {CALIBRATE, 6},
    {RESET, 7},
    {LOW_POWER_MODE, 8},
    {TOWARDS_DRIFT_COMPENSATION_DURATION, 9},
    {AWAY_DRIFT_COMPENSATION_DURATION, 10},
    {DETECTION_INTEGRATOR, 11},
    {RECALIBRATION_DELAY, 12},
    {DRIFT_COMPENSATION_HOLD_DURATION, 13},
    {SLIDER_OR_WHEEL_ENABLE, 14},
    {CHARGE_DURATION, 15},
    {KEY_DETECT_THRESHOLD, 16},
    {KEY_CONTROL, 28},
    {KEY_PULSE_SCALE, 40},
    {KEY_SIGNAL, 52},
    {KEY_REFERENCE, 76},
  };
  const std::map<AT42QTRegister, uint8_t> reg_AT42QT2160 {
    {CHIP_ID, 0},
    {VERSION, 1},
    {STATUS, 2},
    {SUB_REVISION, 7},
    {CALIBRATE, 10},
    {RESET, 11},
    {LOW_POWER_MODE, 12},
    {BURST_REPETITION, 13},
    {TOWARDS_DRIFT_COMPENSATION_DURATION, 15},
    {AWAY_DRIFT_COMPENSATION_DURATION, 16},
    {DETECTION_INTEGRATOR, 17},
    {RECALIBRATION_DELAY, 18},
    {DRIFT_COMPENSATION_HOLD_DURATION, 19},
    {SLIDER_CONTROL, 20},
    {SLIDER_OPTIONS, 21},
    {KEY_CONTROL, 22},
    {KEY_DETECT_THRESHOLD, 38},
    {KEY_BURST_LENGTH, 54},
    {GPIO_GPO_DRIVE, 70},
    {GPIO_DIRECTION, 73},
    {GPIO_GPO_PWM, 74},
    {PWM_LEVEL, 76},
    {GPIO_WAKE, 77},
    {COMMON_CHANGE_KEYS, 78},
    {KEY_SIGNAL, 100},
    {KEY_REFERENCE, 132},
  };

  // -- status bitmaps -- //
  const AT42QTStatusBitmap bit_AT42QT1060 = {255, 255, 7, 0};
  const AT42QTStatusBitmap bit_AT42QT1070 = {0, 6, 7, 8};
  const AT42QTStatusBitmap bit_AT42QT2120 = {0, 6, 7, 8};
  const AT42QTStatusBitmap bit_AT42QT2160 = {255, 6, 255, 7};


  // -- Final Mapping -- //
  //keycount, chipID, addr, from https://github.com/janelia-arduino/AT42QT/tree/09685cb123f61658bafe513609bf38cb96f55c0e/src/AT42QT
  const struct AT42QTSpec spec_AT42QT1060 = { 6, 0x31, 0x12, &reg_AT42QT1060, &bit_AT42QT1060};
  const struct AT42QTSpec spec_AT42QT1070 = { 7, 0x2E, 0x1B, &reg_AT42QT1070, &bit_AT42QT1070};
  const struct AT42QTSpec spec_AT42QT2120 = {12, 0x3E, 0x1C, &reg_AT42QT2120, &bit_AT42QT2120};
  const struct AT42QTSpec spec_AT42QT2160 = {16, 0x11, 0x0D, &reg_AT42QT2160, &bit_AT42QT2160};
} //namespace spec_data

std::map<uint16_t, const AT42QTSpec*> chipnum_to_spec{
    {1060, &spec_data::spec_AT42QT1060},
    {1070, &spec_data::spec_AT42QT1070},
    {2120, &spec_data::spec_AT42QT2120},
    {2160, &spec_data::spec_AT42QT2160}
};

} //namespace at42qt
} //namespace esphome

