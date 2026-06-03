#include "flowiq2200_parser.h"
#include "esphome/core/log.h"
#include <cstdio>

namespace esphome {
namespace kamstrup_wmbus {

static const char *const TAG = "kamstrup_wmbus.flowiq2200";

WMBusMeterData FlowIQ2200Parser::parse(const uint8_t *plaintext, uint8_t length) {
  WMBusMeterData data;
  data.valid = false;
  data.frame_type = "flowiq2200-raw";

  if (plaintext == nullptr || length == 0) {
    ESP_LOGW(TAG, "Empty plaintext - nothing to capture");
    return data;
  }

  data.plaintext_length = length;
  data.frame_marker = (length >= 3) ? plaintext[2] : 0x00;

  // Build a single contiguous hex string (no spaces) that is easy to copy out
  // of the log and paste somewhere for offset analysis. Max plaintext is 64
  // bytes, so 64*2 + 1 fits comfortably.
  char hex_compact[2 * 64 + 1];
  int off = 0;
  for (uint8_t i = 0; i < length && off < (int) sizeof(hex_compact) - 2; i++) {
    off += snprintf(hex_compact + off, sizeof(hex_compact) - off, "%02X", plaintext[i]);
  }

  // Prominent, easy-to-grep capture block. The flowIQ 2200 parser is not yet
  // implemented, so we surface the raw decrypted telegram and stop here.
  ESP_LOGI(TAG, "================ FLOWIQ2200 TELEGRAM CAPTURE ================");
  ESP_LOGI(TAG, "  decrypt OK: %u plaintext bytes, frame marker byte[2]=0x%02X",
           length, data.frame_marker);
  ESP_LOGI(TAG, "  HEX: %s", hex_compact);
  ESP_LOGI(TAG, "  Parser not implemented yet - copy the HEX line above so the");
  ESP_LOGI(TAG, "  field offsets can be mapped. No sensor values published.");
  ESP_LOGI(TAG, "=============================================================");

  // Also emit a spaced, byte-indexed view at DEBUG for manual offset hunting.
  char hex_spaced[3 * 64 + 1];
  off = 0;
  for (uint8_t i = 0; i < length && off < (int) sizeof(hex_spaced) - 3; i++) {
    off += snprintf(hex_spaced + off, sizeof(hex_spaced) - off, "%02X ", plaintext[i]);
  }
  ESP_LOGD(TAG, "  spaced: %s", hex_spaced);

  return data;
}

}  // namespace kamstrup_wmbus
}  // namespace esphome
