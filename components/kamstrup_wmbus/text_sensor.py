"""Text sensor support for Kamstrup wMBUS water meters."""
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import text_sensor
from . import kamstrup_wmbus_ns, KamstrupWMBusComponent

CONF_KAMSTRUP_WMBUS_ID = "kamstrup_wmbus_id"
CONF_INFO_CODES = "info_codes"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_KAMSTRUP_WMBUS_ID): cv.use_id(KamstrupWMBusComponent),
        cv.Optional(CONF_INFO_CODES): text_sensor.text_sensor_schema(
            icon="mdi:alert-circle",
        ),
    }
)


async def to_code(config):
    """Generate C++ code from config."""
    parent = await cg.get_variable(config[CONF_KAMSTRUP_WMBUS_ID])

    if CONF_INFO_CODES in config:
        sens = await text_sensor.new_text_sensor(config[CONF_INFO_CODES])
        cg.add(parent.set_info_codes_sensor(sens))
