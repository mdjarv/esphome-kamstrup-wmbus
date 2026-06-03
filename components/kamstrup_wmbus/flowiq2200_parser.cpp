#include "flowiq2200_parser.h"
#include "esphome/core/log.h"
#include <cstdio>

namespace esphome {
namespace kamstrup_wmbus {

static const char *const TAG = "kamstrup_wmbus.flowiq2200";

// Read a 4-byte little-endian unsigned integer from a buffer.
static uint32_t read_u32_le(const uint8_t *p) {
  return (uint32_t) p[0] | (uint32_t) p[1] << 8 |
         (uint32_t) p[2] << 16 | (uint32_t) p[3] << 24;
}

// Decrypted flowIQ 2200 compact-frame layout (CI 0x79), confirmed against a
// real meter whose display read 404 m3:
//
//   idx 11..14  total volume, uint32 LE, in whole m3 (matched display exactly)
//
// Other fields (target volume, flow/ambient temperature, status) are not yet
// mapped - they are left as "unavailable" (NaN) until confirmed against more
// telegrams. The full plaintext is still logged at DEBUG for that work.
static constexpr uint8_t OFFSET_TOTAL_M3 = 11;

WMBusMeterData FlowIQ2200Parser::parse(const uint8_t *plaintext, uint8_t length) {
  WMBusMeterData data;  // target/temps default to NaN (unavailable)
  data.valid = false;
  data.frame_type = "flowiq2200";

  if (plaintext == nullptr || length < OFFSET_TOTAL_M3 + 4) {
    ESP_LOGW(TAG, "Plaintext too short for flowIQ 2200: %u bytes", length);
    return data;
  }

  data.plaintext_length = length;
  data.frame_marker = plaintext[2];

  // Total volume - confirmed field, value is directly in m3.
  data.total_consumption_m3 = (float) read_u32_le(&plaintext[OFFSET_TOTAL_M3]);
  data.valid = true;

  ESP_LOGI(TAG, "flowIQ 2200: total=%.0f m3", data.total_consumption_m3);

  // Keep dumping the full plaintext (DEBUG) so the remaining fields - target,
  // temperatures, status - can be mapped from more samples.
  char hex[3 * 64 + 1];
  int off = 0;
  for (uint8_t i = 0; i < length && off < (int) sizeof(hex) - 3; i++) {
    off += snprintf(hex + off, sizeof(hex) - off, "%02X ", plaintext[i]);
  }
  ESP_LOGD(TAG, "  plaintext (%u bytes): %s", length, hex);

  return data;
}

}  // namespace kamstrup_wmbus
}  // namespace esphome
