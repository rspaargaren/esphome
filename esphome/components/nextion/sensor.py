import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor, uart
from esphome.const import CONF_COMPONENT_ID, CONF_PAGE_ID, CONF_ID, UNIT_EMPTY, ICON_EMPTY
from . import nextion_ns
from .display import Nextion

DEPENDENCIES = ['display']

CONF_NEXTION_ID = 'nextion_id'
CONF_BUTTON_ID = 'button_id'

NextionSwitch = nextion_ns.class_('NextionSensor', sensor.Sensor, uart.UARTDevice)

CONFIG_SCHEMA = sensor.sensor_schema(UNIT_EMPTY, ICON_EMPTY, 2).extend({
    cv.GenerateID(): cv.declare_id(NextionSwitch),

    cv.GenerateID(CONF_NEXTION_ID): cv.use_id(Nextion),
    cv.Required(CONF_BUTTON_ID): cv.string,
    cv.Required(CONF_PAGE_ID): cv.uint8_t,
    cv.Required(CONF_COMPONENT_ID): cv.uint8_t,
}).extend(uart.UART_DEVICE_SCHEMA)

def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    yield sensor.register_sensor(var, config)
    yield uart.register_uart_device(var, config)

    hub = yield cg.get_variable(config[CONF_NEXTION_ID])
    cg.add(hub.register_sensor_component(var))

    cg.add(var.set_component_id(config[CONF_COMPONENT_ID]))
    cg.add(var.set_page_id(config[CONF_PAGE_ID]))
    cg.add(var.set_device_id(config[CONF_BUTTON_ID]))
