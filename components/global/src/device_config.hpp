#pragma once

/**
 * @brief Device Configuration and Capabilities
 * 
 * This header defines device-specific constants, resolutions, and capabilities
 * for all supported e-reader devices.
 */

#include <cstdint>

/**
 * @struct DeviceCapabilities
 * @brief Defines the capabilities and specifications of a device
 */
struct DeviceCapabilities {
    /// Display width in pixels
    const uint16_t display_width;
    
    /// Display height in pixels
    const uint16_t display_height;
    
    /// Bits per pixel for grayscale (1, 2, or 3)
    const uint8_t grayscale_bits;
    
    /// Number of grayscale levels (2^grayscale_bits)
    const uint8_t grayscale_levels;
    
    /// Display resolution in DPI (dots per inch)
    const uint16_t resolution_dpi;
    
    /// Device has touch screen support
    const bool has_touch_screen;
    
    /// Device has front light support
    const bool has_front_light;
    
    /// Device has battery support
    const bool has_battery;
    
    /// Device has SD card support
    const bool has_sd_card;
    
    /// Device has RTC support
    const bool has_rtc;
    
    /// Device name
    const char * device_name;
    
    /// Device identifier
    const uint8_t device_id;
};

/**
 * @brief Get device capabilities at runtime
 * @return Reference to the DeviceCapabilities structure for the current device
 */
const DeviceCapabilities & get_device_capabilities();

/**
 * @brief Get device capability (safe getter with default)
 * @param index Test whether device is ready
 * @return Display width for current device
 */
inline uint16_t get_display_width() {
    return get_device_capabilities().display_width;
}

/**
 * @brief Get display height
 * @return Display height for current device
 */
inline uint16_t get_display_height() {
    return get_device_capabilities().display_height;
}

/**
 * @brief Get grayscale bits
 * @return Bits per pixel for current device
 */
inline uint8_t get_grayscale_bits() {
    return get_device_capabilities().grayscale_bits;
}

/**
 * @brief Get grayscale levels
 * @return Number of grayscale levels (colors) for current device
 */
inline uint8_t get_grayscale_levels() {
    return get_device_capabilities().grayscale_levels;
}

/**
 * @brief Check if device has touch screen
 * @return true if device has touch screen
 */
inline bool has_touch_screen() {
    return get_device_capabilities().has_touch_screen;
}

/**
 * @brief Check if device has front light
 * @return true if device has front light
 */
inline bool has_front_light() {
    return get_device_capabilities().has_front_light;
}

/**
 * @brief Check if device has battery monitoring
 * @return true if device has battery support
 */
inline bool has_battery() {
    return get_device_capabilities().has_battery;
}

/**
 * @brief Check if device has SD card support
 * @return true if device has SD card
 */
inline bool has_sd_card() {
    return get_device_capabilities().has_sd_card;
}

/**
 * @brief Check if device has RTC
 * @return true if device has real-time clock
 */
inline bool has_rtc() {
    return get_device_capabilities().has_rtc;
}

/**
 * @brief Get device name
 * @return Device name string (e.g., "InkPlate 6", "M5 Paper S3")
 */
inline const char * get_device_name() {
    return get_device_capabilities().device_name;
}

/**
 * @brief Get device ID
 * @return Unique device identifier
 */
inline uint8_t get_device_id() {
    return get_device_capabilities().device_id;
}
