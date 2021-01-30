import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart
from esphome.const import CONF_ID

DEPENDENCIES = ["uart"]
AUTO_LOAD = ["sensor", "binary_sensor", "text_sensor"]
CODEOWNERS = ["@rspaargaren"]
MULTI_CONF = True

CONF_DSMR_ID = "dsmr_id"

dsmr_ns = cg.esphome_ns.namespace("espdsmr")
EspDsmr = dsmr_ns.class_("EspDsmr", cg.PollingComponent, uart.UARTDevice)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(EspDsmr),
        }
    )
    .extend(cv.polling_component_schema("30s"))
    .extend(uart.UART_DEVICE_SCHEMA)
)


def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    yield cg.register_component(var, config)
    yield uart.register_uart_device(var, config)
    cg.add_library("matthijskooijman/Dsmr", "0.1")
