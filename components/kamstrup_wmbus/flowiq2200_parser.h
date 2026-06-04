#pragma once

#include "wmbus_meter_parser.h"
#include <cstdint>

namespace esphome {
namespace kamstrup_wmbus {

/**
 * @brief Parser for Kamstrup flowIQ 2200 wMBUS payloads
 *
 * The flowIQ 2200 uses the same radio/CRC/AES-128-CTR stack as the Multical21
 * but a different decrypted payload layout.
 *
 * Current state: total volume is mapped and confirmed (offset 11, uint32 LE,
 * liters) against a meter display reading 0.404 then 0.571 m3. Target volume,
 * flow/ambient temperature and status are not yet mapped and are reported as
 * unavailable (NaN) until a full frame (marker 0x78), which carries explicit
 * M-Bus DIF/VIF records, lets us map them. Plaintext is logged for that work.
 */
class FlowIQ2200Parser : public WMBusMeterParser {
 public:
  WMBusMeterData parse(const uint8_t *plaintext, uint8_t length) override;
  const char *model_name() const override { return "flowiq2200"; }
};

}  // namespace kamstrup_wmbus
}  // namespace esphome
