import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import binary_sensor
from esphome.const import CONF_ID, CONF_CHANNEL
from . import AT42QTHub, CONF_AT42QT_HUB_ID, at42qt_ns

DEPENDENCIES = ["at42qt"]
AT42QTChannel = at42qt_ns.class_("AT42QTChannel", binary_sensor.BinarySensor)

CONFIG_SCHEMA = (
    binary_sensor.binary_sensor_schema(AT42QTChannel).extend(
        {
            cv.GenerateID(CONF_AT42QT_HUB_ID): cv.use_id(AT42QTHub),
            cv.Required(CONF_CHANNEL): cv.int_range(min=0, max=11),
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_AT42QT_HUB_ID])
    var = await binary_sensor.new_binary_sensor(config)
    cg.add(var.set_channel(config[CONF_CHANNEL]))
    cg.add(hub.register_channel(var))
