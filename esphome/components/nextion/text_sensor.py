from esphome.components import text_sensor
import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.const import CONF_ID

from .display import Nextion
from . import nextion_ns, CONF_NEXTION_ID
from .defines import CONF_NEXTION_COMPONENT, CONF_NEXTION_COMPONENT_NAME

NextionTextSensor = nextion_ns.class_(
    "NextionTextSensor", text_sensor.TextSensor, cg.PollingComponent
)

CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.Required(CONF_NEXTION_COMPONENT): cv.All(
                text_sensor.TEXT_SENSOR_SCHEMA.extend(
                    {
                        cv.GenerateID(): cv.declare_id(NextionTextSensor),
                        cv.GenerateID(CONF_NEXTION_ID): cv.use_id(Nextion),
                        cv.Required(CONF_NEXTION_COMPONENT_NAME): cv.string,
                    }
                ).extend(cv.polling_component_schema("never")),
            ),
        }
    ),
)


def to_code(config):
    conf = config[CONF_NEXTION_COMPONENT]
    hub = yield cg.get_variable(conf[CONF_NEXTION_ID])
    var = cg.new_Pvariable(conf[CONF_ID], hub)
    yield cg.register_component(var, conf)
    yield text_sensor.register_text_sensor(var, conf)

    cg.add(hub.register_textsensor_component(var))

    cg.add(
        var.set_variable_name(
            conf[CONF_NEXTION_COMPONENT_NAME],
            conf[CONF_NEXTION_COMPONENT_NAME] + ".txt",
        )
    )
