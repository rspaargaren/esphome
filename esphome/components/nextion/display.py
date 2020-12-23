import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import display, uart
from esphome.components.http_request import HttpRequestComponent
from esphome.const import CONF_ID, CONF_LAMBDA, CONF_BRIGHTNESS
from . import nextion_ns

DEPENDENCIES = ["uart", "network"]
AUTO_LOAD = ["binary_sensor"]
CONF_HTTPREQUEST_ID = "http_request_id"
CONF_FIRMWARE_URL = "firmware_url"

Nextion = nextion_ns.class_("Nextion", cg.PollingComponent, uart.UARTDevice)
NextionRef = Nextion.operator("ref")

CONFIG_SCHEMA = (
    display.BASIC_DISPLAY_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(Nextion),
            cv.Optional(CONF_HTTPREQUEST_ID): cv.use_id(HttpRequestComponent),
            cv.Optional(CONF_FIRMWARE_URL, default=""): cv.string,
            cv.Optional(CONF_BRIGHTNESS, default=1.0): cv.percentage,
        }
    )
    .extend(cv.polling_component_schema("5s"))
    .extend(uart.UART_DEVICE_SCHEMA)
)


def validate_firmware_section(config):
    url_ = config[CONF_FIRMWARE_URL]
    if config.get(CONF_HTTPREQUEST_ID) and url_.len() == 0:
        raise cv.Invalid("Firmware URL is empty")
    return config


def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    yield cg.register_component(var, config)
    yield uart.register_uart_device(var, config)

    if CONF_BRIGHTNESS in config:
        cg.add(var.set_brightness(config[CONF_BRIGHTNESS]))
    if CONF_LAMBDA in config:
        lambda_ = yield cg.process_lambda(
            config[CONF_LAMBDA], [(NextionRef, "it")], return_type=cg.void
        )
        cg.add(var.set_writer(lambda_))

    if CONF_HTTPREQUEST_ID in config:
        httprequest_pointer = yield cg.get_variable(config[CONF_HTTPREQUEST_ID])
        cg.add(var.set_httprequest(httprequest_pointer))
        cg.add(var.set_firmware_url(config[CONF_FIRMWARE_URL]))

    yield display.register_display(var, config)
