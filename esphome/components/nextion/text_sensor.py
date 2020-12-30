from esphome.components import text_sensor
import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.const import (
    CONF_COMPONENT_ID,
    CONF_PAGE_ID,
    CONF_ID,
)
from . import nextion_ns, CONF_NEXTION_ID
from .defines import CONF_VARIABLE_ID
from .display import Nextion

NextionTextSensor = nextion_ns.class_(
    "NextionTextSensor", text_sensor.TextSensor, cg.PollingComponent
)

CONFIG_SCHEMA = cv.All(
    text_sensor.TEXT_SENSOR_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(NextionTextSensor),
            cv.GenerateID(CONF_NEXTION_ID): cv.use_id(Nextion),
            cv.Optional(CONF_PAGE_ID): cv.uint8_t,
            cv.Optional(CONF_COMPONENT_ID): cv.uint8_t,
            cv.Optional(CONF_VARIABLE_ID): cv.string,
        }
    ).extend(cv.polling_component_schema("60s")),
)


def to_code(config):
    if (
        not config.keys() >= {CONF_COMPONENT_ID, CONF_PAGE_ID}
        and CONF_VARIABLE_ID not in config
    ):
        raise cv.Invalid(
            "At least {CONF_COMPONENT_ID} and {CONF_PAGE_ID} or {CONF_VARIABLE_ID} needs to be set\n"
            + "{CONF_COMPONENT_ID} & {CONF_PAGE_ID} is used on data coming from the Nextion\n"
            + ", if you arent sending from the nextion this can be skipped\n"
            + "{CONF_VARIABLE_ID} is used to get/poll the data from the nextion"
        )

    hub = yield cg.get_variable(config[CONF_NEXTION_ID])
    var = cg.new_Pvariable(config[CONF_ID], hub)
    yield cg.register_component(var, config)
    yield text_sensor.register_text_sensor(var, config)

    if CONF_COMPONENT_ID in config:
        cg.add(var.set_component_id(config[CONF_COMPONENT_ID]))
    else:
        cg.add(var.set_component_id(-1))

    if CONF_PAGE_ID in config:
        cg.add(var.set_page_id(config[CONF_PAGE_ID]))
    else:
        cg.add(var.set_page_id(-1))

    if CONF_VARIABLE_ID in config:
        cg.add(var.set_variable_id(config[CONF_VARIABLE_ID]))