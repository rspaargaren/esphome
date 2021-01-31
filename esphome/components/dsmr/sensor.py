import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import CONF_ID, UNIT_EMPTY, ICON_EMPTY, CONF_COMPONENT_ID
from . import nextion_ns, CONF_NEXTION_ID
from .display import Nextion

from .defines import (
    CONF_NEXTION_VARIABLE_NAME,
    CONF_NEXTION_COMPONENT_NAME,
    CONF_NEXTION_PRECISION,
    CONF_NEXTION_WAVE_CHANNEL_ID,
    CONF_NEXTION_WAVE_MODE,
    CONF_NEXTION_WAVE_MAX_VALUE,
)

WAVE_MODES = {
    "OFF": 0,
    "STATE": 1,
    "TIME": 2,
}

CODEOWNERS = ["@senexcrenshaw", "@rspaargaren"]

DEPENDENCIES = ["display"]

NextionSensor = nextion_ns.class_("NextionSensor", sensor.Sensor, cg.PollingComponent)

CONFIG_SCHEMA = cv.All(
    cv.Schema(
        sensor.sensor_schema(UNIT_EMPTY, ICON_EMPTY, 2)
        .extend(
            {
                cv.GenerateID(): cv.declare_id(NextionSensor),
                cv.GenerateID(CONF_NEXTION_ID): cv.use_id(Nextion),
                cv.Optional(CONF_NEXTION_COMPONENT_NAME): cv.string,
                cv.Optional(CONF_NEXTION_VARIABLE_NAME): cv.string,
                cv.Optional(CONF_NEXTION_PRECISION, default=0): cv.uint8_t,
                cv.Optional(CONF_NEXTION_WAVE_CHANNEL_ID, default=0): cv.uint8_t,
                cv.Optional(CONF_COMPONENT_ID): cv.uint8_t,
                cv.Optional(CONF_NEXTION_WAVE_MAX_VALUE): cv.uint32_t,
                cv.Optional(CONF_NEXTION_WAVE_MODE, default="OFF"): cv.enum(
                    WAVE_MODES, upper=True
                ),
            }
        )
        .extend(cv.polling_component_schema("never"))
    ),
    cv.has_exactly_one_key(
        CONF_COMPONENT_ID, CONF_NEXTION_COMPONENT_NAME, CONF_NEXTION_VARIABLE_NAME
    ),
)


def to_code(config):
    hub = yield cg.get_variable(config[CONF_NEXTION_ID])
    var = cg.new_Pvariable(config[CONF_ID], hub)
    yield cg.register_component(var, config)
    yield sensor.register_sensor(var, config)

    cg.add(hub.register_sensor_component(var))

    if CONF_NEXTION_VARIABLE_NAME in config:
        cg.add(var.set_variable_name(config[CONF_NEXTION_VARIABLE_NAME]))

    if CONF_NEXTION_COMPONENT_NAME in config:
        cg.add(
            var.set_variable_name(
                config[CONF_NEXTION_COMPONENT_NAME],
                config[CONF_NEXTION_COMPONENT_NAME] + ".val",
            )
        )

    if CONF_NEXTION_PRECISION in config:
        cg.add(var.set_precision(config[CONF_NEXTION_PRECISION]))

    if CONF_COMPONENT_ID in config:
        cg.add(var.set_component_id(config[CONF_COMPONENT_ID]))

    if CONF_NEXTION_WAVE_CHANNEL_ID in config:
        cg.add(var.set_wave_channel_id(config[CONF_NEXTION_WAVE_CHANNEL_ID]))

    if CONF_NEXTION_WAVE_MODE in config:
        cg.add(var.set_wave_mode(config[CONF_NEXTION_WAVE_MODE]))

    if CONF_NEXTION_WAVE_MAX_VALUE in config:
        cg.add(var.set_wave_max_value(config[CONF_NEXTION_WAVE_MAX_VALUE]))