from esphome.components import text_sensor
import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.const import CONF_ID
from . import nextion_ns, CONF_NEXTION_ID
from .defines import CONF_COMPONENT_NAME, CONF_VARIABLE_NAME
from .display import Nextion

NextionTextSensor = nextion_ns.class_(
    "NextionTextSensor", text_sensor.TextSensor, cg.PollingComponent
)

CONFIG_SCHEMA = cv.All(
    text_sensor.TEXT_SENSOR_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(NextionTextSensor),
            cv.GenerateID(CONF_NEXTION_ID): cv.use_id(Nextion),
            cv.Optional(CONF_COMPONENT_NAME): cv.string,
            cv.Optional(CONF_VARIABLE_NAME): cv.string,
        }
    ).extend(cv.polling_component_schema("never")),
    cv.has_exactly_one_key(CONF_COMPONENT_NAME, CONF_VARIABLE_NAME),
)


def to_code(config):
    hub = yield cg.get_variable(config[CONF_NEXTION_ID])
    var = cg.new_Pvariable(config[CONF_ID], hub)
    yield cg.register_component(var, config)
    yield text_sensor.register_text_sensor(var, config)

    cg.add(hub.register_textsensor_component(var))

    if CONF_VARIABLE_NAME in config:
        cg.add(var.set_variable_name(config[CONF_VARIABLE_NAME]))

    if CONF_COMPONENT_NAME in config:
        cg.add(
            var.set_variable_name(
                config[CONF_COMPONENT_NAME], config[CONF_COMPONENT_NAME] + ".txt"
            )
        )
