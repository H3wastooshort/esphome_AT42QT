import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c
from esphome.const import CONF_ID, CONF_RESET_PIN, CONF_PULSE_LENGTH

MULTI_CONF = True
DEPENDENCIES = ["i2c"]
CODEOWNERS = ["@h3wastooshort"]

CONF_AT42QT_HUB_ID = "at42qt_hub_id"

CONF_CHANGE_PIN="change_pin"

CONF_CHIP_NUM="chip_number"

#datasheet parameter abbreviations to easier to understand names
CONF_TTD="drift_toward"
CONF_ATD="drift_away"
CONF_DI="deglitch"
CONF_TRD="reject_delay"
CONF_DHT="drift_hold_time"

at42qt_ns = cg.esphome_ns.namespace("at42qt")
AT42QTHub = at42qt_ns.class_("AT42QTHub", cg.Component, i2c.I2CDevice)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(AT42QTHub),
        cv.Optional(CONF_CHIP_NUM, default=2120): cv.int_range(min=0, max=0xFFFF), #TODO: validate that chipnum in map
        cv.Optional(CONF_PULSE_LENGTH, default=0): cv.int_range(min=0, max=0xFF),
        cv.Optional(CONF_TTD, default=20): cv.int_range(min=0, max=127),
        cv.Optional(CONF_ATD, default=5): cv.int_range(min=0, max=127),
        cv.Optional(CONF_DI, default=4): cv.int_range(min=1, max=32),
        cv.Optional(CONF_TRD, default=255): cv.int_range(min=0, max=255),
        cv.Optional(CONF_DHT, default=25): cv.int_range(min=0, max=255),
    }
).extend(cv.COMPONENT_SCHEMA).extend(i2c.i2c_device_schema(0x1C))

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID], config[CONF_CHIP_NUM], config[CONF_PULSE_LENGTH], config[CONF_TTD], config[CONF_ATD], config[CONF_DI], config[CONF_TRD], config[CONF_DHT])
    #TODO: pinconfig
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)