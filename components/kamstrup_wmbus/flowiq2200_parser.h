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
 * Current state: NO fields are reliably mapped yet. Comparing two compact
 * telegrams showed the value at [11..12] is a per-transmission counter, not the
 * total (it changed 404 -> 568 between captures). All readings are therefore
 * reported as unavailable (NaN) until a FULL frame (marker 0x78), which carries
 * explicit M-Bus DIF/VIF records, lets us map fields unambiguously. The parser
 * logs every plaintext (and flags full frames) to support that mapping.
 */
class FlowIQ2200Parser : public WMBusMeterParser {
 public:
  WMBusMeterData parse(const uint8_t *plaintext, uint8_t length) override;
  const char *model_name() const override { return "flowiq2200"; }
};

}  // namespace kamstrup_wmbus
}  // namespace esphome
