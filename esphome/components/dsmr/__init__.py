import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart
from esphome.core import coroutine
from esphome.const import (
    CONF_ID,
    CONF_LAMBDA,
    CONF_BRIGHTNESS,
    CONF_TRIGGER_ID,
)

dsmr_ns = cg.esphome_ns.namespace("dsmr")
Dsmr = dsmr_ns.class_("Dsmr", cg.PollingComponent, uart.UARTDevice)
dsmr_ref = Dsmr.operator("ref")

CONF_DSMR_ID = "dsmr_id"

CODEOWNERS = ["@rspaargaren"]

DEPENDENCIES = ["uart", "network"]

AUTO_LOAD = ["binary_sensor", "sensor", "text_sensor"]

CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(DSMR),
            cv.Optional(CONF_ON_FINISHED_PLAYBACK): automation.validate_automation(
                {
                    cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(
                        DFPlayerFinishedPlaybackTrigger
                    ),
                }
            ),
        }
    )
    .extend(cv.polling_component_schema("30s"))
    .extend(uart.UART_DEVICE_SCHEMA)
)


def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    yield cg.register_component(var, config)
    yield uart.register_uart_device(var, config)

    if CONF_BRIGHTNESS in config:
        cg.add(var.set_brightness(config[CONF_BRIGHTNESS]))
    if CONF_LAMBDA in config:
        lambda_ = yield cg.process_lambda(
            config[CONF_LAMBDA], [(nextion_ref, "it")], return_type=cg.void
        )
        cg.add(var.set_writer(lambda_))

    if CONF_TFT_URL in config:
        cg.add(var.set_tft_url(config[CONF_TFT_URL]))

    yield display.register_display(var, config)

    yield setup_nextion_(var, config)