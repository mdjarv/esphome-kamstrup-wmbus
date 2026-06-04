#include "flowiq2200_parser.h"
#include "esphome/core/log.h"
#include <cstdio>
#include <cmath>

namespace esphome {
namespace kamstrup_wmbus {

static const char *const TAG = "kamstrup_wmbus.flowiq2200";

// Frame marker at plaintext[2]: 0x78 = full frame (explicit DIF/VIF records),
// anything else (seen: 0x79) = compact frame (fields referenced by signature).
static constexpr uint8_t FRAME_MARKER_FULL = 0x78;

WMBusMeterData FlowIQ2200Parser::parse(const uint8_t *plaintext, uint8_t length) {
  WMBusMeterData data;  // target/temps default to NaN (unavailable)
  data.total_consumption_m3 = NAN;  // not yet reliably mapped - publish nothing

  if (plaintext == nullptr || length < 3) {
    ESP_LOGW(TAG, "Plaintext too short for flowIQ 2200: %u bytes", length);
    return data;
  }

  data.plaintext_length = length;
  data.frame_marker = plaintext[2];
  bool is_full = (plaintext[2] == FRAME_MARKER_FULL);
  data.frame_type = is_full ? "flowiq2200-full" : "flowiq2200-compact";
  data.valid = true;  // we received a real frame; total just isn't mapped yet

  // [11..12] increments every transmission - a counter, NOT the total. Logged
  // only as a diagnostic so its drift is visible.
  uint16_t tx_counter = (length > 12) ? (plaintext[11] | (plaintext[12] << 8)) : 0;

  if (is_full) {
    ESP_LOGI(TAG, "**************************************************************");
    ESP_LOGI(TAG, "*** flowIQ 2200 FULL FRAME (0x78) - needed to map fields! ***");
    ESP_LOGI(TAG, "*** Please copy the plaintext line below.                  ***");
    ESP_LOGI(TAG, "**************************************************************");
  } else {
    ESP_LOGI(TAG, "flowIQ 2200: compact frame (tx_counter~%u); total not mapped yet",
             tx_counter);
  }

  // Always dump the full plaintext so frames (especially full ones) can be mapped.
  char hex[3 * 64 + 1];
  int off = 0;
  for (uint8_t i = 0; i < length && off < (int) sizeof(hex) - 3; i++) {
    off += snprintf(hex + off, sizeof(hex) - off, "%02X ", plaintext[i]);
  }
  ESP_LOGI(TAG, "  plaintext (%u bytes): %s", length, hex);

  return data;
}

}  // namespace kamstrup_wmbus
}  // namespace esphome
