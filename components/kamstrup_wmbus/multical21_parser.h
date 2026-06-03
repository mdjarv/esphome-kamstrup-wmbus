#pragma once

#include "wmbus_meter_parser.h"
#include <cstdint>
#include <string>

namespace esphome {
namespace kamstrup_wmbus {

/**
 * @brief Parser for Kamstrup Multical21 wMBUS payloads
 *
 * Extracts meter readings from decrypted Multical21 data packets.
 * Supports both compact and long frame formats (distinguished by the
 * frame marker at byte 2 of the decrypted plaintext).
 *
 * Responsibility: Pure data extraction for the Multical21 payload layout.
 */
class Multical21Parser : public WMBusMeterParser {
 public:
  WMBusMeterData parse(const uint8_t *plaintext, uint8_t length) override;
  const char *model_name() const override { return "multical21"; }

 private:
  /**
   * @brief Detect if plaintext is a long frame format
   *
   * Long frames have different field positions than compact frames.
   *
   * @param plaintext Decrypted payload data
   * @return true if long frame (0x78 marker), false if compact frame
   */
  bool is_long_frame_(const uint8_t *plaintext);

  /**
   * @brief Decode meter status code to human-readable string
   *
   * @param info_codes Raw status byte from meter
   * @return Status string (e.g., "normal", "leak", "code_0x05")
   */
  std::string decode_status_(uint8_t info_codes);
};

}  // namespace kamstrup_wmbus
}  // namespace esphome
