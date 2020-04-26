import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import light, output, uart
from esphome.const import CONF_OUTPUT_ID, CONF_OUTPUT, CONF_PAGE_ID, CONF_ID, CONF_COMPONENT_ID
from . import nextion_ns
from .display import Nextion

DEPENDENCIES = ['display']

CONF_NEXTION_ID = 'nextion_id'
CONF_BUTTON_ID = 'button_id'

NextionLightDummy = nextion_ns.class_('NextionLightDummy', light.LightOutput, uart.UARTDevice)

CONFIG_SCHEMA = light.BRIGHTNESS_ONLY_LIGHT_SCHEMA.extend({
    cv.GenerateID(CONF_OUTPUT_ID): cv.declare_id(NextionLightDummy),
    #cv.Required(CONF_OUTPUT): cv.use_id(output.FloatOutput),
    cv.GenerateID(CONF_NEXTION_ID): cv.use_id(Nextion),
    cv.Required(CONF_BUTTON_ID): cv.string,
    cv.Required(CONF_PAGE_ID): cv.uint8_t,
    cv.Required(CONF_COMPONENT_ID): cv.uint8_t,
}).extend(uart.UART_DEVICE_SCHEMA)


def to_code(config):
    var = cg.new_Pvariable(config[CONF_OUTPUT_ID])
    yield light.register_light(var, config)
    yield uart.register_uart_device(var, config)
    
    hub = yield cg.get_variable(config[CONF_NEXTION_ID])
    cg.add(hub.register_light_component(var))

    cg.add(var.set_component_id(config[CONF_COMPONENT_ID]))
    cg.add(var.set_page_id(config[CONF_PAGE_ID]))
    cg.add(var.set_device_id(config[CONF_BUTTON_ID]))