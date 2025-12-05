import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import binary_sensor
from esphome.const import CONF_ID, CONF_CHANNEL
from . import AT42QTHub, CONF_AT42QT_HUB_ID

DEPENDENCIES = ["at42qt"]

CONFIG_SCHEMA = (
    binary_sensor.binary_sensor_schema()
    .extend(
        {
            cv.GenerateID(CONF_AT42QT_HUB_ID): cv.use_id(AT42QTHub),
            cv.Required(CONF_CHANNEL): cv.int_range(min=0, max=11),
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
)


async def to_code(config):
    paren = await cg.get_variable(config[CONF_AT42QT_HUB_ID])
    var = cg.new_Pvariable(config[CONF_ID])
    cg.add(var.set_channel(config[CONF_CHANNEL]))
    await binary_sensor.register_binary_sensor(var, config)
    cg.add(paren.register_binary_sensor(var))