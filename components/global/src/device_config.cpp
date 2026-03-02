#include "device_config.hpp"
#include "global.hpp"

/**
 * Device capability specifications for all supported devices
 */

#if INKPLATE_6
  static const DeviceCapabilities inkplate_6_caps = {
      .display_width = 1024,
      .display_height = 758,
      .grayscale_bits = 3,
      .grayscale_levels = 8,
      .resolution_dpi = 166,
      .has_touch_screen = false,
      .has_front_light = false,
      .has_battery = true,
      .has_sd_card = true,
      .has_rtc = true,
      .device_name = "InkPlate 6",
      .device_id = 1
  };

#elif INKPLATE_10
  static const DeviceCapabilities inkplate_10_caps = {
      .display_width = 1200,
      .display_height = 825,
      .grayscale_bits = 3,
      .grayscale_levels = 8,
      .resolution_dpi = 150,
      .has_touch_screen = false,
      .has_front_light = false,
      .has_battery = true,
      .has_sd_card = true,
      .has_rtc = true,
      .device_name = "InkPlate 10",
      .device_id = 2
  };

#elif INKPLATE_6FLICK
  static const DeviceCapabilities inkplate_6plus_caps = {
      .display_width = 1024,
      .display_height = 758,
      .grayscale_bits = 3,
      .grayscale_levels = 8,
      .resolution_dpi = 212,
      .has_touch_screen = true,
      .has_front_light = false,
      .has_battery = true,
      .has_sd_card = true,
      .has_rtc = true,
      .device_name = "InkPlate 6 Flick",
      .device_id = 3
  };

#elif INKPLATE_6PLUS || INKPLATE_6PLUS_V2
  static const DeviceCapabilities inkplate_6plus_caps = {
      .display_width = 1024,
      .display_height = 758,
      .grayscale_bits = 3,
      .grayscale_levels = 8,
      .resolution_dpi = 212,
      .has_touch_screen = true,
      .has_front_light = true,
      .has_battery = true,
      .has_sd_card = true,
      .has_rtc = true,
      .device_name = "InkPlate 6 Plus",
      .device_id = 3
  };

#elif M5_PAPER_S3
  static const DeviceCapabilities m5_paper_s3_caps = {
      .display_width = 600,
      .display_height = 960,
      .grayscale_bits = 2,
      .grayscale_levels = 4,
      .resolution_dpi = 165,
      .has_touch_screen = true,
      .has_front_light = false,
      .has_battery = true,
      .has_sd_card = true,
      .has_rtc = true,
      .device_name = "M5 Paper S3",
      .device_id = 4
  };

#endif

/**
 * @brief Get device capabilities at runtime
 * 
 * Returns the DeviceCapabilities structure for the currently configured device.
 * Device selection is determined at compile time via preprocessor defines.
 * 
 * @return const reference to DeviceCapabilities for current device
 * @throws None - configuration checked at compile time
 */
const DeviceCapabilities & get_device_capabilities() {
  #if INKPLATE_6
    return inkplate_6_caps;
  #elif INKPLATE_10
    return inkplate_10_caps;
  #elif INKPLATE_6PLUS || INKPLATE_6PLUS_V2 || INKPLATE_6FLICK
    return inkplate_6plus_caps;
  #elif M5_PAPER_S3
    return m5_paper_s3_caps;
  #else
    #error "No device defined. Set one of: INKPLATE_6, INKPLATE_10, INKPLATE_6PLUS, INKPLATE_6PLUS_V2, INKPLATE_6FLICK, M5_PAPER_S3"
  #endif
}
