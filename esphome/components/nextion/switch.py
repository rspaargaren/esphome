import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import switch
from esphome.const import CONF_ID

from . import nextion_ns, CONF_NEXTION_ID
from .display import Nextion

from .defines import (
    CONF_NEXTION_VARIABLE_NAME,
    CONF_NEXTION_COMPONENT_NAME,
    CONF_HASS_COMPONENT_NAME,
)

CODEOWNERS = ["@senexcrenshaw", "@rspaargaren"]

NextionSwitch = nextion_ns.class_("NextionSwitch", switch.Switch, cg.PollingComponent)

CONFIG_SCHEMA = cv.All(
    cv.Schema(
        switch.SWITCH_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(NextionSwitch),
                cv.GenerateID(CONF_NEXTION_ID): cv.use_id(Nextion),
                cv.Optional(CONF_NEXTION_COMPONENT_NAME): cv.string,
                cv.Optional(CONF_NEXTION_VARIABLE_NAME): cv.string,
                cv.Optional(CONF_HASS_COMPONENT_NAME, default="none"): cv.string,
            }
        ).extend(cv.polling_component_schema("never"))
    ),
    cv.has_exactly_one_key(CONF_NEXTION_COMPONENT_NAME, CONF_NEXTION_VARIABLE_NAME),
)


def to_code(config):
    hub = yield cg.get_variable(config[CONF_NEXTION_ID])
    var = cg.new_Pvariable(config[CONF_ID], hub)
    yield cg.register_component(var, config)
    yield switch.register_switch(var, config)
    cg.add(hub.register_switch_component(var))

    if CONF_NEXTION_VARIABLE_NAME in config:
        cg.add(var.set_variable_name(config[CONF_NEXTION_VARIABLE_NAME]))

    if CONF_NEXTION_COMPONENT_NAME in config:
        cg.add(
            var.set_variable_name(
                config[CONF_NEXTION_COMPONENT_NAME],
                config[CONF_NEXTION_COMPONENT_NAME] + ".val",
            )
        )

    if CONF_HASS_COMPONENT_NAME in config:
        cg.add(var.set_hass_name(config[CONF_HASS_COMPONENT_NAME]))
