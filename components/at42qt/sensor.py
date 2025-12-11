import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import CONF_ID, CONF_CHANNEL, UNIT_EMPTY, ENTITY_CATEGORY_DIAGNOSTIC
from . import AT42QTHub, CONF_AT42QT_HUB_ID, at42qt_ns

DEPENDENCIES = ["at42qt"]
CODEOWNERS = ["@h3wastooshort"]

AT42QTDebug = at42qt_ns.class_("AT42QTDebug", cg.PollingComponent)

CONF_SENSOR_SIGNAL = "signal"
CONF_SENSOR_REFERENCE = "reference"

ICON_SIGNAL_VARIANT = "mdi:signal-variant"
ICON_WAVEFORM = "mdi:waveform"

UNIT_COUNTS = "#"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(AT42QTDebug),
        cv.GenerateID(CONF_AT42QT_HUB_ID): cv.use_id(AT42QTHub),
        cv.Required(CONF_CHANNEL): cv.int_range(min=0, max=15), #TODO: make this based on hub
        cv.Optional(CONF_SENSOR_SIGNAL): sensor.sensor_schema(
            unit_of_measurement=UNIT_COUNTS,
            icon=ICON_SIGNAL_VARIANT,
            accuracy_decimals=0,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(CONF_SENSOR_REFERENCE): sensor.sensor_schema(
            unit_of_measurement=UNIT_COUNTS,
            icon=ICON_WAVEFORM,
            accuracy_decimals=0,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
    }
).extend(cv.polling_component_schema("10s"))

async def to_code(config):    
    var = cg.new_Pvariable(config[CONF_ID], config[CONF_CHANNEL])
    await cg.register_component(var, config)

    if CONF_SENSOR_SIGNAL in config:
        sens = await sensor.new_sensor(config[CONF_SENSOR_SIGNAL])
        cg.add(var.set_sensor_sig(sens))
    if CONF_SENSOR_REFERENCE in config:
        sens = await sensor.new_sensor(config[CONF_SENSOR_REFERENCE])
        cg.add(var.set_sensor_ref(sens))

    hub = await cg.get_variable(config[CONF_AT42QT_HUB_ID])  
    cg.add(hub.register_debug(var))
