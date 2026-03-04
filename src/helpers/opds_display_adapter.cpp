// Copyright (c) 2026 OPDS Display Adapter
//
// MIT License. Look at file licenses.txt for details.

#include "helpers/opds_display_adapter.hpp"
#include "screen.hpp"
#include "global.hpp"
#include "esp_log.h"
#include <cmath>
#include <algorithm>

static const char* TAG = "M5EPaperPanel";

M5EPaperPanel::M5EPaperPanel()
  : screen(&Screen::get_singleton())
{
  LOG_D("M5EPaperPanel adapter initialized");
}

M5EPaperPanel::~M5EPaperPanel()
{
}

uint8_t
M5EPaperPanel::convert_color_to_4bit(uint8_t color_8bit)
{
  // Convert from 8-bit grayscale (0-255) to 4-bit (0-15)
  // Simple linear scaling
  return (color_8bit >> 4) & 0x0F;
}

void
M5EPaperPanel::set_pixel(uint16_t x, uint16_t y, uint8_t color)
{
  if (x >= Screen::get_width() || y >= Screen::get_height()) {
    return;
  }

  uint8_t color_4bit = convert_color_to_4bit(color);
  
  // Use colorize_region to set a single pixel
  Dim pixel_dim(1, 1);
  Pos pixel_pos(x, y);
  screen->colorize_region(pixel_dim, pixel_pos, color_4bit);
}

void
M5EPaperPanel::fill_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t color)
{
  if (width == 0 || height == 0) {
    return;
  }

  // Clamp to screen boundaries
  uint16_t x_end = std::min((uint16_t)(x + width), Screen::get_width());
  uint16_t y_end = std::min((uint16_t)(y + height), Screen::get_height());

  if (x >= Screen::get_width() || y >= Screen::get_height()) {
    return;
  }

  width = x_end - x;
  height = y_end - y;

  uint8_t color_4bit = convert_color_to_4bit(color);

  // Use Screen's colorize_region to fill the rectangle
  Dim rect_dim(width, height);
  Pos rect_pos(x, y);
  screen->colorize_region(rect_dim, rect_pos, color_4bit);
}

void
M5EPaperPanel::draw_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t color)
{
  if (width == 0 || height == 0) {
    return;
  }

  // Draw four lines to create a rectangle border
  // Top line
  draw_line(x, y, x + width - 1, y, color);
  
  // Bottom line
  draw_line(x, y + height - 1, x + width - 1, y + height - 1, color);
  
  // Left line
  draw_line(x, y, x, y + height - 1, color);
  
  // Right line
  draw_line(x + width - 1, y, x + width - 1, y + height - 1, color);
}

void
M5EPaperPanel::draw_line_axis_aligned(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t color)
{
  // Horizontal line
  if (y1 == y2) {
    uint16_t x_min = std::min(x1, x2);
    uint16_t x_max = std::max(x1, x2);
    for (uint16_t x = x_min; x <= x_max; x++) {
      set_pixel(x, y1, color);
    }
  }
  // Vertical line
  else if (x1 == x2) {
    uint16_t y_min = std::min(y1, y2);
    uint16_t y_max = std::max(y1, y2);
    for (uint16_t y = y_min; y <= y_max; y++) {
      set_pixel(x1, y, color);
    }
  }
}

void
M5EPaperPanel::draw_line_bresenham(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t color)
{
  // Bresenham line algorithm for diagonal lines
  int dx = abs((int)x2 - (int)x1);
  int dy = abs((int)y2 - (int)y1);
  int sx = x1 < x2 ? 1 : -1;
  int sy = y1 < y2 ? 1 : -1;
  int err = dx - dy;

  int x = x1;
  int y = y1;

  while (true) {
    set_pixel(x, y, color);

    if (x == (int)x2 && y == (int)y2) {
      break;
    }

    int e2 = 2 * err;
    if (e2 > -dy) {
      err -= dy;
      x += sx;
    }
    if (e2 < dx) {
      err += dx;
      y += sy;
    }
  }
}

void
M5EPaperPanel::draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t color)
{
  // Clamp to screen boundaries
  // For simplicity, just clamp endpoints
  x1 = std::min(x1, (uint16_t)(Screen::get_width() - 1));
  y1 = std::min(y1, (uint16_t)(Screen::get_height() - 1));
  x2 = std::min(x2, (uint16_t)(Screen::get_width() - 1));
  y2 = std::min(y2, (uint16_t)(Screen::get_height() - 1));

  // Check if line is axis-aligned
  if (x1 == x2 || y1 == y2) {
    draw_line_axis_aligned(x1, y1, x2, y2, color);
  } else {
    draw_line_bresenham(x1, y1, x2, y2, color);
  }
}

void
M5EPaperPanel::draw_string(uint16_t x, uint16_t y, const char* text, uint16_t size, uint8_t color)
{
  if (!text || !text[0]) {
    return;
  }

  // Convert size to approximate character dimensions
  // Typical monospace: 1 char = ~0.6 * font_size width
  uint16_t char_width = std::max(4u, size / 2);
  uint16_t char_height = size;

  // For now, use a simple rasterization approach
  // Draw a placeholder rectangle for each character to show text area
  const char* ptr = text;
  uint16_t x_offset = 0;

  while (*ptr != '\0') {
    // Draw a simple rectangle outline for each character to represent text
    // This is a placeholder - in a real implementation, we'd use glyphs
    draw_rect(x + x_offset, y, char_width, char_height, color);
    
    x_offset += char_width;
    ptr++;

    // Stop if we would go off-screen
    if (x + x_offset >= Screen::get_width()) {
      break;
    }
  }

  LOG_D("Rendered text '%s' at (%d, %d) with size %d", text, x, y, size);
}

void
M5EPaperPanel::displayWindow()
{
  // Update the e-ink display with current frame buffer
  screen->update(true);  // true = use partial update for faster refresh
  LOG_D("Display window updated");
}
