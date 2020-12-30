from esphome.components.ade7953.sensor import DEPENDENCIES
import esphome.codegen as cg

nextion_ns = cg.esphome_ns.namespace("nextion")
CONF_NEXTION_ID = "nextion_id"
DEPENDENCIES = ["binary_sensor", "switch", "sensor", "text_sensor"]
