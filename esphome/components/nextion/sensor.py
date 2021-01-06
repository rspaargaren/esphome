import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.core import coroutine
from esphome.const import CONF_ID, UNIT_EMPTY, ICON_EMPTY, UNIT_PERCENT, ICON_PERCENT
from . import nextion_ns, CONF_NEXTION_ID
from .display import Nextion

from .defines import (
    CONF_NEXTION_VARIABLE_NAME,
    CONF_NEXTION_COMPONENT_NAME,
    CONF_NEXTION_VARIABLE,
    CONF_NEXTION_COMPONENT,
)

DEPENDENCIES = ["display"]

NextionSensor = nextion_ns.class_("NextionSensor", sensor.Sensor, cg.PollingComponent)

CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(CONF_NEXTION_ID): cv.use_id(Nextion),
            cv.Optional(CONF_NEXTION_COMPONENT): cv.All(
                sensor.sensor_schema(UNIT_EMPTY, ICON_EMPTY, 2)
                .extend(
                    {
                        cv.GenerateID(): cv.declare_id(NextionSensor),
                        cv.Required(CONF_NEXTION_COMPONENT_NAME): cv.string,
                    }
                )
                .extend(cv.polling_component_schema("never")),
            ),
            cv.Optional(CONF_NEXTION_VARIABLE): sensor.sensor_schema(
                UNIT_EMPTY, ICON_EMPTY, 2
            )
            .extend(
                {
                    cv.GenerateID(): cv.declare_id(NextionSensor),
                    cv.Required(CONF_NEXTION_VARIABLE_NAME): cv.string,
                }
            )
            .extend(cv.polling_component_schema("never")),
        }
    ),
    cv.has_exactly_one_key(CONF_NEXTION_COMPONENT, CONF_NEXTION_VARIABLE),
)


@coroutine
def setup_conf(hub, config, funcName, is_component=None):
    var = cg.new_Pvariable(config[CONF_ID], hub)
    yield cg.register_component(var, config)
    yield sensor.register_sensor(var, config)

    cg.add(hub.register_sensor_component(var))

    func = getattr(var, funcName)

    if is_component is None:
        param = config[CONF_NEXTION_VARIABLE_NAME]
        cg.add(func(param))
    else:
        param = config[CONF_NEXTION_COMPONENT_NAME]
        cg.add(func(param, param + ".val"))


def to_code(config):
    hub = yield cg.get_variable(config[CONF_NEXTION_ID])

    if CONF_NEXTION_COMPONENT in config:
        yield setup_conf(hub, config[CONF_NEXTION_COMPONENT], "set_variable_name", 1)

    if CONF_NEXTION_VARIABLE in config:
        yield setup_conf(hub, config[CONF_NEXTION_VARIABLE], "set_variable_name")
