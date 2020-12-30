import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import switch, uart
from esphome.const import CONF_COMPONENT_ID, CONF_PAGE_ID, CONF_ID
from . import nextion_ns, CONF_NEXTION_ID
from .display import Nextion

DEPENDENCIES = ["display"]

CONF_BUTTON_ID = "button_id"
CONF_VARIABLE_ID = "variable_id"

NextionSwitch = nextion_ns.class_(
    "NextionSwitch", switch.Switch, cg.Component, uart.UARTDevice
)

CONFIG_SCHEMA = cv.All(
    switch.SWITCH_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(NextionSwitch),
            cv.GenerateID(CONF_NEXTION_ID): cv.use_id(Nextion),
            cv.Optional(CONF_BUTTON_ID): cv.string,
            cv.Optional(CONF_VARIABLE_ID): cv.string,
            cv.Optional(CONF_PAGE_ID): cv.uint8_t,
            cv.Optional(CONF_COMPONENT_ID): cv.uint8_t,
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
    .extend(uart.UART_DEVICE_SCHEMA)
)


def to_code(config):
    if not config.keys() >= {CONF_COMPONENT_ID, CONF_PAGE_ID} and (
        CONF_VARIABLE_ID not in config or CONF_BUTTON_ID not in config
    ):
        raise cv.Invalid(
            "At least {CONF_COMPONENT_ID} and {CONF_PAGE_ID} or {CONF_VARIABLE_ID} or {CONF_BUTTON_ID} needs to be set\n"
            + "{CONF_COMPONENT_ID} & {CONF_PAGE_ID} is used on data coming from the Nextion\n"
            + ", if you arent sending from the nextion this can be skipped\n"
            + "{CONF_VARIABLE_ID} is used to get/poll the data from a variable in the nextion"
            + "{CONF_BUTTON_ID} is used to get/poll the data from a component in the nextion"
        )
    hub = yield cg.get_variable(config[CONF_NEXTION_ID])
    var = cg.new_Pvariable(config[CONF_ID], hub)
    yield cg.register_component(var, config)
    yield switch.register_switch(var, config)
    yield uart.register_uart_device(var, config)

    cg.add(hub.register_switch_component(var))

    if CONF_COMPONENT_ID in config:
        cg.add(var.set_component_id(config[CONF_COMPONENT_ID]))
    else:
        cg.add(var.set_component_id(-1))

    if CONF_PAGE_ID in config:
        cg.add(var.set_page_id(config[CONF_PAGE_ID]))
    else:
        cg.add(var.set_page_id(-1))

    if CONF_BUTTON_ID in config:
        cg.add(var.set_variable_id(config[CONF_BUTTON_ID] + ".val"))

    if CONF_VARIABLE_ID in config:
        cg.add(var.set_variable_id(config[CONF_VARIABLE_ID]))
