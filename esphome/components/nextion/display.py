import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation
from esphome.components import display, uart
from esphome.core import coroutine
from esphome.const import (
    CONF_ID,
    CONF_LAMBDA,
    CONF_BRIGHTNESS,
    CONF_TRIGGER_ID,
)
from . import nextion_ns
from .defines import CONF_ON_SLEEP, CONF_ON_WAKE

DEPENDENCIES = ["uart", "network"]
AUTO_LOAD = ["binary_sensor", "switch", "sensor", "text_sensor"]

CONF_HTTP_REQUEST_ID = "http_request_id"
CONF_TFT_URL = "tft_url"

Nextion = nextion_ns.class_("Nextion", cg.PollingComponent, uart.UARTDevice)
NextionRef = Nextion.operator("ref")

SleepTrigger = nextion_ns.class_("SleepTrigger", automation.Trigger.template())
WakeTrigger = nextion_ns.class_("WakeTrigger", automation.Trigger.template())

CONFIG_SCHEMA = (
    display.BASIC_DISPLAY_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(Nextion),
            cv.Optional(CONF_TFT_URL, default=""): cv.string,
            cv.Optional(CONF_BRIGHTNESS, default=1.0): cv.percentage,
            cv.Optional(CONF_ON_SLEEP): automation.validate_automation(
                {
                    cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(SleepTrigger),
                }
            ),
            cv.Optional(CONF_ON_WAKE): automation.validate_automation(
                {
                    cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(WakeTrigger),
                }
            ),
        }
    )
    .extend(cv.polling_component_schema("5s"))
    .extend(uart.UART_DEVICE_SCHEMA)
)


@coroutine
def setup_nextion_(var, config):
    for conf in config.get(CONF_ON_SLEEP, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        yield automation.build_automation(trigger, [], conf)
    for conf in config.get(CONF_ON_WAKE, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        yield automation.build_automation(trigger, [], conf)


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

    if CONF_TFT_URL in config:
        cg.add(var.set_tft_url(config[CONF_TFT_URL]))

    yield display.register_display(var, config)

    yield setup_nextion_(var, config)
