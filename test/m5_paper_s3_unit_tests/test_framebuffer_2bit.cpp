/**
 * @file test_framebuffer_2bit.cpp
 * @brief Unit tests for 2-bit grayscale frame buffer conversion
 * 
 * Tests frame buffer conversions between 1-bit monochrome and 2-bit grayscale
 * for M5 Paper S3 display (4 grayscale levels: 0=white, 1=light_gray, 2=dark_gray, 3=black)
 */

#include <iostream>
#include <cassert>
#include <cstring>
#include <stdint.h>

// Mock frame buffer structures for testing
struct FrameBuffer1Bit {
    uint8_t * data;
    uint16_t width;
    uint16_t height;
    
    FrameBuffer1Bit(uint16_t w, uint16_t h) : width(w), height(h) {
        // 1 bit per pixel: width * height / 8 bytes
        uint32_t size = (w * h) / 8;
        data = new uint8_t[size];
        memset(data, 0, size);
    }
    
    ~FrameBuffer1Bit() { delete[] data; }
    
    void set_pixel(uint16_t x, uint16_t y, uint8_t color) {
        uint32_t byte_idx = (y * width + x) / 8;
        uint8_t bit_pos = 7 - ((y * width + x) % 8);
        if (color) {
            data[byte_idx] |= (1 << bit_pos);
        } else {
            data[byte_idx] &= ~(1 << bit_pos);
        }
    }
};

struct FrameBuffer2Bit {
    uint8_t * data;
    uint16_t width;
    uint16_t height;
    
    FrameBuffer2Bit(uint16_t w, uint16_t h) : width(w), height(h) {
        // 2 bits per pixel: width * height / 4 bytes
        uint32_t size = (w * h) / 4;
        data = new uint8_t[size];
        memset(data, 0, size);
    }
    
    ~FrameBuffer2Bit() { delete[] data; }
    
    void set_pixel(uint16_t x, uint16_t y, uint8_t color) {
        // 4 pixels per byte (2 bits each)
        // color: 0=white, 1=light_gray, 2=dark_gray, 3=black
        uint32_t byte_idx = (y * width + x) / 4;
        uint8_t pixel_in_byte = (x) % 4;
        uint8_t shift = (3 - pixel_in_byte) << 1;  // (3-pos)*2
        data[byte_idx] = (data[byte_idx] & ~(0x03 << shift)) | ((color & 0x03) << shift);
    }
    
    uint8_t get_pixel(uint16_t x, uint16_t y) {
        uint32_t byte_idx = (y * width + x) / 4;
        uint8_t pixel_in_byte = (x) % 4;
        uint8_t shift = (3 - pixel_in_byte) << 1;
        return (data[byte_idx] >> shift) & 0x03;
    }
};

// Conversion function: 1-bit monochrome -> 2-bit grayscale
void convert_1bit_to_2bit(const FrameBuffer1Bit & src, FrameBuffer2Bit & dst) {
    assert(src.width == dst.width);
    assert(src.height == dst.height);
    
    for (uint16_t y = 0; y < src.height; y++) {
        for (uint16_t x = 0; x < src.width; x++) {
            // Get 1-bit value (0 or 1)
            uint32_t byte_idx = (y * src.width + x) / 8;
            uint8_t bit_pos = 7 - ((y * src.width + x) % 8);
            uint8_t bit_val = (src.data[byte_idx] >> bit_pos) & 1;
            
            // Convert: 0 (black) -> 3 (black), 1 (white) -> 0 (white)
            uint8_t grayscale = bit_val ? 0 : 3;
            dst.set_pixel(x, y, grayscale);
        }
    }
}

// Test 1: Basic pixel setting on 2-bit frame buffer
void test_2bit_pixel_setting() {
    std::cout << "Test 1: 2-bit pixel setting..." << std::endl;
    
    FrameBuffer2Bit fb(8, 8);  // 8x8 = 64 pixels = 16 bytes
    
    // Set specific pixels with different grayscale levels
    fb.set_pixel(0, 0, 0);  // white
    fb.set_pixel(1, 0, 1);  // light gray
    fb.set_pixel(2, 0, 2);  // dark gray
    fb.set_pixel(3, 0, 3);  // black
    
    // Verify they were set correctly
    assert(fb.get_pixel(0, 0) == 0);
    assert(fb.get_pixel(1, 0) == 1);
    assert(fb.get_pixel(2, 0) == 2);
    assert(fb.get_pixel(3, 0) == 3);
    
    std::cout << "  ✓ Passed: Pixel values set and retrieved correctly" << std::endl;
}

// Test 2: Memory layout validation for 2-bit frame buffer
void test_2bit_memory_layout() {
    std::cout << "Test 2: 2-bit memory layout validation..." << std::endl;
    
    FrameBuffer2Bit fb(600, 960);  // M5 Paper S3 resolution
    uint32_t expected_size = (600 * 960) / 4;  // 144,000 bytes
    
    // Verify memory allocation
    assert(fb.data != nullptr);
    assert(fb.width == 600);
    assert(fb.height == 960);
    
    std::cout << "  ✓ Expected buffer size: " << expected_size << " bytes (~" 
              << (float)expected_size / 1024 << " KB)" << std::endl;
    
    std::cout << "  ✓ Passed: Memory layout validated" << std::endl;
}

// Test 3: 1-bit to 2-bit conversion
void test_1bit_to_2bit_conversion() {
    std::cout << "Test 3: 1-bit to 2-bit conversion..." << std::endl;
    
    FrameBuffer1Bit src(8, 8);
    FrameBuffer2Bit dst(8, 8);
    
    // Set alternating pattern in 1-bit buffer
    for (uint16_t x = 0; x < 8; x++) {
        src.set_pixel(x, 0, x % 2);  // alternating 0, 1, 0, 1, ...
    }
    
    // Convert to 2-bit
    convert_1bit_to_2bit(src, dst);
    
    // Verify conversion (1 -> 0 white, 0 -> 3 black)
    for (uint16_t x = 0; x < 8; x++) {
        uint8_t expected = (x % 2) ? 0 : 3;
        uint8_t actual = dst.get_pixel(x, 0);
        assert(actual == expected);
    }
    
    std::cout << "  ✓ Passed: 1-bit to 2-bit conversion successful" << std::endl;
}

// Test 4: Byte boundary handling in 2-bit buffers
void test_2bit_byte_boundaries() {
    std::cout << "Test 4: Byte boundary handling..." << std::endl;
    
    FrameBuffer2Bit fb(4, 1);  // Single row of 4 pixels = exactly 1 byte
    
    // Fill entire byte with pattern
    fb.set_pixel(0, 0, 0);  // bits 7-6: 00
    fb.set_pixel(1, 0, 1);  // bits 5-4: 01
    fb.set_pixel(2, 0, 2);  // bits 3-2: 10
    fb.set_pixel(3, 0, 3);  // bits 1-0: 11
    
    // Expected byte: 0b00011011 = 0x1B
    assert(fb.data[0] == 0x1B);
    
    // Verify round-trip
    assert(fb.get_pixel(0, 0) == 0);
    assert(fb.get_pixel(1, 0) == 1);
    assert(fb.get_pixel(2, 0) == 2);
    assert(fb.get_pixel(3, 0) == 3);
    
    std::cout << "  ✓ Passed: Byte layout correct (0x" << std::hex << (int)fb.data[0] 
              << std::dec << ")" << std::endl;
}

// Test 5: Large buffer performance check
void test_2bit_large_buffer() {
    std::cout << "Test 5: Large buffer (M5 Paper S3 full resolution)..." << std::endl;
    
    FrameBuffer2Bit fb(600, 960);  // Full M5 Paper S3 resolution
    
    // Set a test pattern (checkerboard)
    for (uint16_t y = 0; y < 960; y++) {
        for (uint16_t x = 0; x < 600; x++) {
            uint8_t color = ((x >> 3) + (y >> 3)) % 4;
            fb.set_pixel(x, y, color);
        }
    }
    
    // Verify corner pixels
    assert(fb.get_pixel(0, 0) == 0);
    assert(fb.get_pixel(599, 959) == ((599 >> 3) + (959 >> 3)) % 4);
    
    std::cout << "  ✓ Passed: Large buffer handled correctly (600x960 pixels)" << std::endl;
}

// Main test runner
int main() {
    std::cout << "\n=== M5 Paper S3 Frame Buffer 2-Bit Unit Tests ===" << std::endl;
    std::cout << "Testing 2-bit grayscale frame buffer implementation\n" << std::endl;
    
    try {
        test_2bit_pixel_setting();
        test_2bit_memory_layout();
        test_1bit_to_2bit_conversion();
        test_2bit_byte_boundaries();
        test_2bit_large_buffer();
        
        std::cout << "\n✅ All tests passed!\n" << std::endl;
        return 0;
    } catch (const std::exception & e) {
        std::cerr << "\n❌ Test failed: " << e.what() << std::endl;
        return 1;
    }
}
