// Copyright (c) 2026 OPDS Display Adapter
//
// MIT License. Look at file licenses.txt for details.

#pragma once

#include <cstdint>
#include <string>

// Forward declaration
class Screen;

/**
 * @brief M5EPaperPanel adapter for EPub-InkPlate Screen class
 * 
 * This adapter implements the M5EPaperPanel interface expected by
 * opds_ui_manager, translating calls to the actual Screen class.
 * 
 * Color mapping:
 * - 0 = Black
 * - 255 = White
 * - 128 = Gray (mid-tone)
 * 
 * For M5Paper S3 compatibility, colors in OPDS UI are 8-bit grayscale.
 * The adapter maps these to the 4-bit grayscale range supported by Screen (0-15).
 */
class M5EPaperPanel
{
public:
  M5EPaperPanel();
  ~M5EPaperPanel();

  /**
   * @brief Fill a rectangular region with specified color
   * @param x Starting X coordinate
   * @param y Starting Y coordinate
   * @param width Width in pixels
   * @param height Height in pixels
   * @param color Grayscale value (0-255, where 0=black, 255=white)
   */
  void fill_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t color);

  /**
   * @brief Draw a rectangular border (unfilled)
   * @param x Starting X coordinate
   * @param y Starting Y coordinate
   * @param width Width in pixels
   * @param height Height in pixels
   * @param color Grayscale value (0-255)
   */
  void draw_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t color);

  /**
   * @brief Draw a line segment
   * @param x1 Start X coordinate
   * @param y1 Start Y coordinate
   * @param x2 End X coordinate
   * @param y2 End Y coordinate
   * @param color Grayscale value (0-255)
   */
  void draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t color);

  /**
   * @brief Draw text string
   * @param x Starting X coordinate
   * @param y Starting Y coordinate
   * @param text String to render
   * @param size Font size (12, 16, 18, 24 typical values)
   * @param color Grayscale value (0-255)
   * 
   * Note: Basic rasterized text rendering
   */
  void draw_string(uint16_t x, uint16_t y, const char* text, uint16_t size, uint8_t color);

  /**
   * @brief Display the current frame buffer on screen
   * Updates the physical display with current frame buffer contents
   */
  void displayWindow();

private:
  Screen* screen;

  /**
   * @brief Convert 8-bit grayscale to 4-bit format used by Screen
   * Maps 0-255 to 0-15 range
   */
  uint8_t convert_color_to_4bit(uint8_t color_8bit);

  /**
   * @brief Draw a horizontal or vertical line
   * Used by draw_line for axis-aligned lines
   */
  void draw_line_axis_aligned(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t color);

  /**
   * @brief Draw a general line using Bresenham algorithm
   * Used by draw_line for diagonal lines
   */
  void draw_line_bresenham(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t color);

  /**
   * @brief Set a single pixel at specified coordinates
   */
  void set_pixel(uint16_t x, uint16_t y, uint8_t color);
};
