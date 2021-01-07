import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import switch
from esphome.const import CONF_ID
from esphome.core import coroutine

from . import nextion_ns, CONF_NEXTION_ID
from .display import Nextion

from .defines import (
    CONF_NEXTION_VARIABLE_NAME,
    CONF_NEXTION_COMPONENT_NAME,
    CONF_NEXTION_VARIABLE,
    CONF_NEXTION_COMPONENT,
    CONF_NEXTION_HASS_NAME,
)

NextionSwitch = nextion_ns.class_("NextionSwitch", switch.Switch, cg.PollingComponent)

CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(CONF_NEXTION_ID): cv.use_id(Nextion),
            cv.Optional(CONF_NEXTION_HASS_NAME, default="none"): cv.string,
            cv.Optional(CONF_NEXTION_COMPONENT): cv.All(
                switch.SWITCH_SCHEMA.extend(
                    {
                        cv.GenerateID(): cv.declare_id(NextionSwitch),
                        cv.Required(CONF_NEXTION_COMPONENT_NAME): cv.string,
                    }
                ).extend(cv.polling_component_schema("never")),
            ),
            cv.Optional(CONF_NEXTION_VARIABLE): cv.All(
                switch.SWITCH_SCHEMA.extend(
                    {
                        cv.GenerateID(): cv.declare_id(NextionSwitch),
                        cv.Required(CONF_NEXTION_VARIABLE_NAME): cv.string,
                    }
                ).extend(cv.polling_component_schema("never")),
            ),
        }
    ),
    cv.has_exactly_one_key(CONF_NEXTION_COMPONENT, CONF_NEXTION_VARIABLE),
)


@coroutine
def setup_conf(hub, config, funcName, is_component=None):
    var = cg.new_Pvariable(config[CONF_ID], hub)
    yield cg.register_component(var, config)
    yield switch.register_switch(var, config)

    cg.add(hub.register_switch_component(var))
    cg.add(var.set_hass_name(config[CONF_NEXTION_HASS_NAME]))

    func = getattr(var, funcName)

    if is_component is None:
        param = config[CONF_NEXTION_VARIABLE_NAME]
        cg.add(func(param))
    else:
        param = config[CONF_NEXTION_COMPONENT_NAME]
        cg.add(func(param, param + ".val"))


def to_code(config):
    hub = yield cg.get_variable(config[CONF_NEXTION_ID])
    if CONF_NEXTION_VARIABLE in config:
        yield setup_conf(hub, config[CONF_NEXTION_VARIABLE], "set_variable_name")

    if CONF_NEXTION_COMPONENT in config:
        yield setup_conf(hub, config[CONF_NEXTION_COMPONENT], "set_variable_name", 1)
