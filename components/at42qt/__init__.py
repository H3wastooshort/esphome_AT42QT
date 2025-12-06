import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c
from esphome.const import CONF_ID, CONF_RESET_PIN, CONF_PULSE_LENGTH

MULTI_CONF = True
DEPENDENCIES = ["i2c"]
CODEOWNERS = ["@h3wastooshort"]

CONF_AT42QT_HUB_ID = "at42qt_hub_id"

CONF_CHANGE_PIN="change_pin"

at42qt_ns = cg.esphome_ns.namespace("at42qt")
AT42QTHub = at42qt_ns.class_("AT42QTHub", cg.Component, i2c.I2CDevice)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(AT42QTHub),
        cv.Optional(CONF_PULSE_LENGTH, default=0): cv.int_range(min=0, max=0xFF),
    }
).extend(cv.COMPONENT_SCHEMA).extend(i2c.i2c_device_schema(0x1C))

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID], config[CONF_PULSE_LENGTH])
    #TODO: pinconfig
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)