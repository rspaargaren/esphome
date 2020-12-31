import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import (
    CONF_ID,
    UNIT_EMPTY,
    ICON_EMPTY,
)
from . import nextion_ns, CONF_NEXTION_ID
from .display import Nextion
from .defines import CONF_VARIABLE_NAME, CONF_COMPONENT_NAME

DEPENDENCIES = ["display"]

NextionSensor = nextion_ns.class_("NextionSensor", sensor.Sensor, cg.PollingComponent)

CONFIG_SCHEMA = cv.All(
    sensor.sensor_schema(UNIT_EMPTY, ICON_EMPTY, 2)
    .extend(
        {
            cv.GenerateID(): cv.declare_id(NextionSensor),
            cv.GenerateID(CONF_NEXTION_ID): cv.use_id(Nextion),
            cv.Optional(CONF_COMPONENT_NAME): cv.string,
            cv.Optional(CONF_VARIABLE_NAME): cv.string,
        }
    )
    .extend(cv.polling_component_schema("never")),
    cv.has_exactly_one_key(CONF_COMPONENT_NAME, CONF_VARIABLE_NAME),
)


def to_code(config):
    hub = yield cg.get_variable(config[CONF_NEXTION_ID])
    var = cg.new_Pvariable(config[CONF_ID], hub)
    yield cg.register_component(var, config)
    yield sensor.register_sensor(var, config)

    cg.add(hub.register_sensor_component(var))

    if CONF_VARIABLE_NAME in config:
        cg.add(var.set_variable_name(config[CONF_VARIABLE_NAME]))

    if CONF_COMPONENT_NAME in config:
        cg.add(
            var.set_variable_name(
                config[CONF_COMPONENT_NAME], config[CONF_COMPONENT_NAME] + ".val"
            )
        )
