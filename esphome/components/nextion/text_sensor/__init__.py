from esphome.components.adc.sensor import AUTO_LOAD
from esphome.components import text_sensor
import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.const import (
    CONF_COMPONENT_ID,
    CONF_PAGE_ID,
    CONF_ID,
)
from . import nextion_ns, CONF_NEXTION_ID
from ..defines import CONF_VARIABLE_ID
from ..display import Nextion

NextionTextSensor = nextion_ns.class_(
    "NextionTextSensor", text_sensor.TextSensor, cg.PollingComponent
)

CONFIG_SCHEMA = cv.All(
    text_sensor.TEXT_SENSOR_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(NextionTextSensor),
            cv.GenerateID(CONF_NEXTION_ID): cv.use_id(Nextion),
            cv.Required(CONF_PAGE_ID): cv.uint8_t,
            cv.Required(CONF_COMPONENT_ID): cv.uint8_t,
            cv.Required(CONF_VARIABLE_ID): cv.string,
        }
    ).extend(cv.polling_component_schema("60s")),
)


def to_code(config):
    hub = yield cg.get_variable(config[CONF_NEXTION_ID])
    var = cg.new_Pvariable(config[CONF_ID], hub)
    yield cg.register_component(var, config)
    yield text_sensor.register_text_sensor(var, config)

    cg.add(var.set_component_id(config[CONF_COMPONENT_ID]))
    cg.add(var.set_page_id(config[CONF_PAGE_ID]))
    cg.add(var.set_device_id(config[CONF_VARIABLE_ID]))
