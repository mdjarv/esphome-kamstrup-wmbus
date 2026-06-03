#pragma once

#include "wmbus_meter_parser.h"
#include <cstdint>

namespace esphome {
namespace kamstrup_wmbus {

/**
 * @brief Parser for Kamstrup flowIQ 2200 wMBUS payloads (WORK IN PROGRESS)
 *
 * The flowIQ 2200 uses the same radio/CRC/AES-128-CTR stack as the Multical21
 * but a different decrypted payload layout, so the field offsets must be mapped
 * from a real decrypted telegram before readings can be extracted.
 *
 * Until that mapping is known, parse() does NOT guess field positions. Instead
 * it dumps the full decrypted plaintext to the log in a copy-paste-friendly
 * form (see the "FLOWIQ2200 TELEGRAM CAPTURE" block) and returns an invalid
 * result so no bogus sensor values are published.
 *
 * Capture procedure:
 *   1. Configure `meter_model: flowiq2200` with the meter's real meter_id + aes_key.
 *   2. Let it run; copy a few of the logged hex telegrams.
 *   3. Use those to fill in the real field offsets here.
 */
class FlowIQ2200Parser : public WMBusMeterParser {
 public:
  WMBusMeterData parse(const uint8_t *plaintext, uint8_t length) override;
  const char *model_name() const override { return "flowiq2200"; }
};

}  // namespace kamstrup_wmbus
}  // namespace esphome
