import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c
from esphome.const import CONF_ID

MULTI_CONF = True
DEPENDENCIES = ["i2c"]

CONF_AT42QT_HUB_ID = "at42qt_hub_id"

empty_sensor_hub_ns = cg.esphome_ns.namespace("at42qt")
AT42QTHub = empty_sensor_hub_ns.class_("AT42QTHub", cg.Component, i2c.I2CDevice)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(AT42QTHub),
    }
).extend(cv.COMPONENT_SCHEMA).extend(i2c.i2c_device_schema(0x1C))

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    #TODO: pinconfig
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)