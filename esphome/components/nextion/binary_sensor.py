import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import binary_sensor
from esphome.core import coroutine
from esphome.const import CONF_COMPONENT_ID, CONF_PAGE_ID, CONF_ID
from . import nextion_ns, CONF_NEXTION_ID
from .display import Nextion
from .defines import (
    CONF_NEXTION_VARIABLE_NAME,
    CONF_NEXTION_COMPONENT_NAME,
    CONF_NEXTION_VARIABLE,
    CONF_NEXTION_COMPONENT,
)

DEPENDENCIES = ["display"]

NextionTouchComponent = nextion_ns.class_(
    "NextionTouchComponent", binary_sensor.BinarySensor
)

NextionBinarySensor = nextion_ns.class_(
    "NextionBinarySensor", binary_sensor.BinarySensor, cg.PollingComponent
)

CONFIG_SCHEMA = cv.All(
    binary_sensor.BINARY_SENSOR_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(NextionTouchComponent),
            cv.GenerateID(CONF_NEXTION_ID): cv.use_id(Nextion),
            cv.Optional(CONF_PAGE_ID): cv.uint8_t,
            cv.Optional(CONF_COMPONENT_ID): cv.uint8_t,
            cv.Optional(CONF_NEXTION_COMPONENT): cv.All(
                binary_sensor.BINARY_SENSOR_SCHEMA.extend(
                    {
                        cv.GenerateID(): cv.declare_id(NextionBinarySensor),
                        cv.Required(CONF_NEXTION_COMPONENT_NAME): cv.string,
                    }
                ).extend(cv.polling_component_schema("never")),
            ),
            cv.Optional(CONF_NEXTION_VARIABLE): cv.All(
                binary_sensor.BINARY_SENSOR_SCHEMA.extend(
                    {
                        cv.GenerateID(): cv.declare_id(NextionBinarySensor),
                        cv.Required(CONF_NEXTION_VARIABLE_NAME): cv.string,
                    }
                ).extend(cv.polling_component_schema("never")),
            ),
        }
    ),
    cv.has_at_least_one_key(
        CONF_PAGE_ID, CONF_COMPONENT_ID, CONF_NEXTION_COMPONENT, CONF_NEXTION_VARIABLE
    ),
)


@coroutine
def setup_conf(hub, config, funcName, is_component=None):
    var = cg.new_Pvariable(config[CONF_ID], hub)
    yield binary_sensor.register_binary_sensor(var, config)
    yield cg.register_component(var, config)
    cg.add(hub.register_binarysensor_component(var))

    func = getattr(var, funcName)

    if is_component is None:
        param = config[CONF_NEXTION_VARIABLE_NAME]
        cg.add(func(param))
    else:
        param = config[CONF_NEXTION_COMPONENT_NAME]
        cg.add(func(param, param + ".val"))


def to_code(config):
    hub = yield cg.get_variable(config[CONF_NEXTION_ID])

    if config.keys() >= {CONF_PAGE_ID, CONF_COMPONENT_ID}:
        if CONF_NEXTION_COMPONENT in config or CONF_NEXTION_VARIABLE in config:
            raise cv.Invalid(
                "For Nextion Touch Component only {CONF_PAGE_ID} "
                + "and {CONF_VARIABLE_ID} should be set"
            )
        var = cg.new_Pvariable(config[CONF_ID])
        yield binary_sensor.register_binary_sensor(var, config)
        cg.add(hub.register_touch_component(var))
        cg.add(var.set_component_id(config[CONF_COMPONENT_ID]))
        cg.add(var.set_page_id(config[CONF_PAGE_ID]))
    elif config.keys() >= {CONF_PAGE_ID, CONF_COMPONENT_ID}:
        raise cv.Invalid(
            "For Nextion Binary Sensor only {CONF_NEXTION_COMPONENT} "
            + "or {CONF_NEXTION_VARIABLE} should be set "
            + "not {CONF_PAGE_ID} and/or {CONF_COMPONENT_ID}"
        )
    elif CONF_NEXTION_COMPONENT in config:
        yield setup_conf(hub, config[CONF_NEXTION_COMPONENT], "set_variable_name", 1)

    elif CONF_NEXTION_VARIABLE in config:
        yield setup_conf(hub, config[CONF_NEXTION_VARIABLE], "set_variable_name")
