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
 * Current state: the total volume field is mapped and confirmed against a real
 * meter. Target volume, flow/ambient temperature and status are not yet mapped
 * and are reported as unavailable (NaN). The full plaintext is still logged at
 * DEBUG so those remaining fields can be mapped from more telegrams.
 */
class FlowIQ2200Parser : public WMBusMeterParser {
 public:
  WMBusMeterData parse(const uint8_t *plaintext, uint8_t length) override;
  const char *model_name() const override { return "flowiq2200"; }
};

}  // namespace kamstrup_wmbus
}  // namespace esphome
