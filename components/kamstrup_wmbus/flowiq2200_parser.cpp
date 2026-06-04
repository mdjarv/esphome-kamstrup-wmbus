#include "flowiq2200_parser.h"
#include "esphome/core/log.h"
#include <cstdio>
#include <cmath>

namespace esphome {
namespace kamstrup_wmbus {

static const char *const TAG = "kamstrup_wmbus.flowiq2200";

// Frame marker at plaintext[2]: 0x78 = full frame (explicit DIF/VIF records),
// anything else (seen: 0x79) = compact frame.
static constexpr uint8_t FRAME_MARKER_FULL = 0x78;

// Total volume: uint32 LE at offset 11, in liters (1e-3 m3, M-Bus VIF 0x13).
// Confirmed against a real meter whose display read 0.404 then 0.571 m3 while
// this field read 404 then 568. Other fields (target, temperatures, status)
// are not yet mapped and stay unavailable (NaN) pending a full frame.
static constexpr uint8_t OFFSET_TOTAL_L = 11;

static uint32_t read_u32_le(const uint8_t *p) {
  return (uint32_t) p[0] | (uint32_t) p[1] << 8 |
         (uint32_t) p[2] << 16 | (uint32_t) p[3] << 24;
}

WMBusMeterData FlowIQ2200Parser::parse(const uint8_t *plaintext, uint8_t length) {
  WMBusMeterData data;  // target/temps default to NaN (unavailable)

  if (plaintext == nullptr || length < OFFSET_TOTAL_L + 4) {
    ESP_LOGW(TAG, "Plaintext too short for flowIQ 2200: %u bytes", length);
    return data;
  }

  data.plaintext_length = length;
  data.frame_marker = plaintext[2];
  bool is_full = (plaintext[2] == FRAME_MARKER_FULL);
  data.frame_type = is_full ? "flowiq2200-full" : "flowiq2200-compact";

  // Total volume - liters on the wire, convert to m3.
  data.total_consumption_m3 = read_u32_le(&plaintext[OFFSET_TOTAL_L]) / 1000.0f;
  data.valid = true;

  ESP_LOGI(TAG, "flowIQ 2200: total=%.3f m3 (%s frame)",
           data.total_consumption_m3, is_full ? "full" : "compact");

  // A full frame (0x78) carries explicit DIF/VIF records - capture it to map
  // the remaining fields (target, temperatures, status).
  if (is_full) {
    ESP_LOGI(TAG, "*** flowIQ 2200 FULL FRAME (0x78) - copy the plaintext below ***");
  }

  // Dump the full plaintext so the remaining fields can be mapped from samples.
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
