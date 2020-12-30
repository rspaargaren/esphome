import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import (
    CONF_COMPONENT_ID,
    CONF_PAGE_ID,
    CONF_ID,
    UNIT_EMPTY,
    ICON_EMPTY,
)
from . import nextion_ns, CONF_NEXTION_ID
from .display import Nextion
from .defines import CONFIG_RESTORE_FROM_NEXTION, CONF_VARIABLE_ID

DEPENDENCIES = ["display"]

NextionSensor = nextion_ns.class_("NextionSensor", sensor.Sensor)

CONFIG_SCHEMA = sensor.sensor_schema(UNIT_EMPTY, ICON_EMPTY, 2).extend(
    {
        cv.GenerateID(): cv.declare_id(NextionSensor),
        cv.GenerateID(CONF_NEXTION_ID): cv.use_id(Nextion),
        cv.Required(CONF_PAGE_ID): cv.uint8_t,
        cv.Required(CONF_COMPONENT_ID): cv.uint8_t,
        cv.Optional(CONFIG_RESTORE_FROM_NEXTION): cv.boolean,
        cv.Optional(CONF_VARIABLE_ID): cv.string,
    }
)


def to_code(config):
    if CONFIG_RESTORE_FROM_NEXTION in config and config[CONFIG_RESTORE_FROM_NEXTION]:
        if CONF_VARIABLE_ID not in config:
            raise cv.Invalid(
                "{CONF_VARIABLE_ID} is required if {CONFIG_RESTORE_FROM_NEXTION} is set"
            )
    hub = yield cg.get_variable(config[CONF_NEXTION_ID])
    var = cg.new_Pvariable(config[CONF_ID], hub)
    yield sensor.register_sensor(var, config)

    cg.add(hub.register_sensor_component(var))

    cg.add(var.set_component_id(config[CONF_COMPONENT_ID]))
    cg.add(var.set_page_id(config[CONF_PAGE_ID]))

    if CONF_VARIABLE_ID in config:
        cg.add(var.set_device_id(config[CONF_VARIABLE_ID]))

    if CONFIG_RESTORE_FROM_NEXTION in config:
        cg.add(var.set_device_id(config[CONF_VARIABLE_ID]))