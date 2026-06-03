"""ESPHome component for Kamstrup wMBUS water meters (Multical21, flowIQ 2200) with CC1101 radio."""
import esphome.codegen as cg
from esphome.components import spi

CODEOWNERS = ["@mdjarv"]
kamstrup_wmbus_ns = cg.esphome_ns.namespace("kamstrup_wmbus")
KamstrupWMBusComponent = kamstrup_wmbus_ns.class_(
    "KamstrupWMBusComponent", cg.PollingComponent, spi.SPIDevice
)

# Meter model selection -> picks the C++ payload parser. Mirrors
# `enum class MeterModel` in wmbus_meter_parser.h.
MeterModel = kamstrup_wmbus_ns.enum("MeterModel", is_class=True)
METER_MODELS = {
    "multical21": MeterModel.MULTICAL21,
    "flowiq2200": MeterModel.FLOWIQ2200,
}
