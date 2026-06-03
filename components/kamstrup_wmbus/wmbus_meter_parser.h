#pragma once

#include <cstdint>
#include <cmath>
#include <string>

namespace esphome {
namespace kamstrup_wmbus {

/**
 * @brief Selects which meter-specific parser the component instantiates.
 *
 * The radio, CRC, and AES-128-CTR decryption layers are identical across
 * Kamstrup meters - only the decrypted payload layout differs. Each value
 * here maps to a concrete WMBusMeterParser implementation.
 */
enum class MeterModel : uint8_t {
  MULTICAL21 = 0,
  FLOWIQ2200 = 1,
};

/**
 * @brief Parsed meter data structure
 *
 * Data Transfer Object (DTO) holding all meter readings extracted
 * from a decrypted wMBUS packet. Shared across all meter models -
 * every supported Kamstrup meter reports this same set of fields.
 */
struct WMBusMeterData {
  float total_consumption_m3;    // Total water consumption in cubic meters
  float target_consumption_m3;   // Target/billing consumption in cubic meters (NaN = unavailable)
  float flow_temperature_c;      // Flow temperature in degrees Celsius (NaN = unavailable)
  float ambient_temperature_c;   // Ambient temperature in degrees Celsius (NaN = unavailable)
  std::string status;            // Human-readable meter status (e.g., "normal", "leak")
  bool valid;                    // True if parsing succeeded, false on error

  // Frame analysis fields
  std::string frame_type;        // "compact" or "long" - for debugging/analysis
  uint8_t plaintext_length;      // Length of decrypted plaintext in bytes
  uint8_t frame_marker;          // Byte 2 of plaintext (frame format indicator)

  // Constructor with default invalid state
  WMBusMeterData() :
    total_consumption_m3(0.0f),
    target_consumption_m3(NAN),
    flow_temperature_c(NAN),
    ambient_temperature_c(NAN),
    status("unknown"),
    valid(false),
    frame_type("unknown"),
    plaintext_length(0),
    frame_marker(0x00) {}
};

/**
 * @brief Abstract interface for meter-specific payload parsers (strategy pattern)
 *
 * The component owns one concrete parser, chosen from the configured MeterModel.
 * Implementations turn a decrypted plaintext buffer into a WMBusMeterData.
 *
 * Responsibility: Pure data extraction - no hardware, crypto, or ESPHome
 * component dependencies (logging is allowed).
 */
class WMBusMeterParser {
 public:
  virtual ~WMBusMeterParser() = default;

  /**
   * @brief Parse decrypted plaintext into meter readings
   *
   * @param plaintext Decrypted payload data
   * @param length Length of plaintext in bytes
   * @return WMBusMeterData structure with parsed values (check .valid flag)
   */
  virtual WMBusMeterData parse(const uint8_t *plaintext, uint8_t length) = 0;

  /**
   * @brief Short human-readable name of the meter model this parser handles.
   */
  virtual const char *model_name() const = 0;
};

}  // namespace kamstrup_wmbus
}  // namespace esphome
