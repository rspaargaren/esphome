from esphome.components import text_sensor
import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.const import CONF_ID

from .display import Nextion
from . import nextion_ns, CONF_NEXTION_ID
from .defines import (
    CONF_NEXTION_COMPONENT_NAME,
    CONF_HASS_COMPONENT_NAME,
)

NextionTextSensor = nextion_ns.class_(
    "NextionTextSensor", text_sensor.TextSensor, cg.PollingComponent
)

CONFIG_SCHEMA = cv.All(
    cv.Schema(
        text_sensor.TEXT_SENSOR_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(NextionTextSensor),
                cv.GenerateID(CONF_NEXTION_ID): cv.use_id(Nextion),
                cv.Required(CONF_NEXTION_COMPONENT_NAME): cv.string,
                cv.Optional(CONF_HASS_COMPONENT_NAME, default="none"): cv.string,
            }
        ).extend(cv.polling_component_schema("never")),
    )
)


def to_code(config):
    hub = yield cg.get_variable(config[CONF_NEXTION_ID])
    var = cg.new_Pvariable(config[CONF_ID], hub)
    yield cg.register_component(var, config)
    yield text_sensor.register_text_sensor(var, config)

    cg.add(hub.register_textsensor_component(var))

    cg.add(
        var.set_variable_name(
            config[CONF_NEXTION_COMPONENT_NAME],
            config[CONF_NEXTION_COMPONENT_NAME] + ".txt",
        )
    )

    cg.add(var.set_hass_name(config[CONF_HASS_COMPONENT_NAME]))
