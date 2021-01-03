import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.core import coroutine
from esphome.const import (
    CONF_ID,
    UNIT_EMPTY,
    ICON_EMPTY,
)
from . import nextion_ns, CONF_NEXTION_ID
from .display import Nextion

from .defines import (
    CONF_NEXTION_VARIABLE_NAME,
    CONF_NEXTION_COMPONENT_NAME,
    CONF_NEXTION_VARIABLE,
    CONF_NEXTION_COMPONENT,
    CONF_READONLY,
)

DEPENDENCIES = ["display"]

NextionSensor = nextion_ns.class_("NextionSensor", sensor.Sensor, cg.PollingComponent)
NextionSensorNonPolling = nextion_ns.class_(
    "NextionSensor", sensor.Sensor, cg.Component
)

CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.Optional(CONF_NEXTION_COMPONENT): cv.All(
                sensor.sensor_schema(UNIT_EMPTY, ICON_EMPTY, 2)
                .extend(
                    {
                        cv.GenerateID(): cv.declare_id(NextionSensor),
                        cv.GenerateID(CONF_NEXTION_ID): cv.use_id(Nextion),
                        cv.Required(CONF_NEXTION_COMPONENT_NAME): cv.string,
                        cv.Optional(CONF_READONLY, default=False): cv.boolean,
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
                    cv.GenerateID(CONF_NEXTION_ID): cv.use_id(Nextion),
                    cv.Required(CONF_NEXTION_VARIABLE_NAME): cv.string,
                    cv.Optional(CONF_READONLY, default=False): cv.boolean,
                }
            )
            .extend(cv.polling_component_schema("never")),
        }
    ),
    cv.has_exactly_one_key(CONF_NEXTION_COMPONENT, CONF_NEXTION_VARIABLE),
)


@coroutine
def setup_conf(config, funcName, is_component=None):
    # for key2, value in config.items():
    #     print("KEY: ", key2, " VALUE: ", value)

    hub = yield cg.get_variable(config[CONF_NEXTION_ID])
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

    cg.add(var.set_is_readonly(config[CONF_READONLY]))


def to_code(config):
    if CONF_NEXTION_VARIABLE in config:
        yield setup_conf(config[CONF_NEXTION_VARIABLE], "set_variable_name")
        return

    if CONF_NEXTION_COMPONENT in config:
        yield setup_conf(config[CONF_NEXTION_COMPONENT], "set_variable_name", 1)
        return
