/**
 * @file test_gt911_touch.cpp
 * @brief Unit tests for GT911 capacitive touch controller
 * 
 * Tests GT911 touch input processing for M5 Paper S3
 */

#include <iostream>
#include <cassert>
#include <cstring>
#include <stdint.h>
#include <vector>

// Mock GT911 touch data structures
struct TouchPoint {
    uint16_t x;
    uint16_t y;
    uint8_t id;
    uint16_t pressure;
};

struct GT911RawTouchData {
    uint8_t status;           // Status register at 0x814E
    uint8_t touch_data[40];   // Raw touch data buffer
};

// Mock GT911 controller for testing
class GT911Mock {
private:
    static const uint16_t MAX_TOUCH_POINTS = 2;
    GT911RawTouchData raw_data;
    
public:
    GT911Mock() {
        memset(&raw_data, 0, sizeof(raw_data));
    }
    
    // Load raw register data (simulating I2C read)
    void load_raw_data(const uint8_t * data, size_t len) {
        assert(len <= sizeof(raw_data));
        memcpy(&raw_data, data, len);
    }
    
    // Get number of touch points from status register
    uint8_t get_touch_count() {
        return raw_data.status & 0x0F;  // Bits 0-3 contain touch count
    }
    
    // Check if data is valid (bit 7 set)
    bool is_data_ready() {
        return (raw_data.status & 0x80) != 0;  // Bit 7 indicates data ready
    }
    
    // Parse touch data and return touch points
    std::vector<TouchPoint> parse_touch_data() {
        std::vector<TouchPoint> touches;
        
        if (!is_data_ready()) {
            return touches;  // No valid data
        }
        
        uint8_t count = get_touch_count();
        if (count > MAX_TOUCH_POINTS) {
            count = MAX_TOUCH_POINTS;  // Cap at max
        }
        
        // Each touch point is 8 bytes
        for (uint8_t i = 0; i < count; i++) {
            uint8_t * touch_ptr = &raw_data.touch_data[i * 8];
            
            TouchPoint point;
            point.id = touch_ptr[0];
            // X: little-endian 16-bit at offset 1-2
            point.x = touch_ptr[1] | (touch_ptr[2] << 8);
            // Y: little-endian 16-bit at offset 3-4
            point.y = touch_ptr[3] | (touch_ptr[4] << 8);
            // Pressure: 16-bit at offset 5-6
            point.pressure = touch_ptr[5] | (touch_ptr[6] << 8);
            
            touches.push_back(point);
        }
        
        return touches;
    }
};

// Test 1: Status register interpretation
void test_status_register() {
    std::cout << "Test 1: Status register parsing..." << std::endl;
    
    GT911Mock gt911;
    
    // Test no touch: status = 0x00
    uint8_t data1[] = { 0x00, 0, 0, 0, 0 };
    gt911.load_raw_data(data1, sizeof(data1));
    assert(gt911.get_touch_count() == 0);
    assert(!gt911.is_data_ready());
    
    // Test single touch with ready bit: status = 0x81 (ready + 1 touch)
    uint8_t data2[] = { 0x81, 0, 0, 0, 0 };
    gt911.load_raw_data(data2, sizeof(data2));
    assert(gt911.get_touch_count() == 1);
    assert(gt911.is_data_ready());
    
    // Test two touches with ready bit: status = 0x82
    uint8_t data3[] = { 0x82, 0, 0, 0, 0 };
    gt911.load_raw_data(data3, sizeof(data3));
    assert(gt911.get_touch_count() == 2);
    assert(gt911.is_data_ready());
    
    std::cout << "  ✓ Passed: Status register correctly parsed" << std::endl;
}

// Test 2: Single touch point parsing
void test_single_touch_parsing() {
    std::cout << "Test 2: Single touch point parsing..." << std::endl;
    
    GT911Mock gt911;
    
    // Construct raw touch data
    uint8_t raw_data[] = {
        0x81,           // Status: ready + 1 touch
        // Touch point 0: ID, X_low, X_high, Y_low, Y_high, P_low, P_high, reserved
        0x01,           // Touch ID = 1
        0x10, 0x01,     // X = 0x0110 = 272 (little-endian)
        0x20, 0x02,     // Y = 0x0220 = 544 (little-endian)
        0xFF, 0x00,     // Pressure = 255
        0x00            // Reserved
    };
    
    gt911.load_raw_data(raw_data, sizeof(raw_data));
    auto touches = gt911.parse_touch_data();
    
    assert(touches.size() == 1);
    assert(touches[0].id == 1);
    assert(touches[0].x == 272);
    assert(touches[0].y == 544);
    assert(touches[0].pressure == 255);
    
    std::cout << "  ✓ Passed: Single touch point (272, 544) with pressure 255" << std::endl;
}

// Test 3: Multiple touch points parsing
void test_multiple_touch_parsing() {
    std::cout << "Test 3: Multiple touch points parsing..." << std::endl;
    
    GT911Mock gt911;
    
    // Construct raw touch data with 2 touches
    uint8_t raw_data[] = {
        0x82,           // Status: ready + 2 touches
        // Touch point 0
        0x00,           // Touch ID = 0
        0x00, 0x01,     // X = 0x0100 = 256
        0x00, 0x02,     // Y = 0x0200 = 512
        0x80, 0x00,     // Pressure = 128
        0x00,           // Reserved
        // Touch point 1
        0x01,           // Touch ID = 1
        0x00, 0x02,     // X = 0x0200 = 512
        0x00, 0x03,     // Y = 0x0300 = 768
        0x40, 0x00,     // Pressure = 64
        0x00,           // Reserved
    };
    
    gt911.load_raw_data(raw_data, sizeof(raw_data));
    auto touches = gt911.parse_touch_data();
    
    assert(touches.size() == 2);
    
    // First touch
    assert(touches[0].id == 0);
    assert(touches[0].x == 256);
    assert(touches[0].y == 512);
    assert(touches[0].pressure == 128);
    
    // Second touch
    assert(touches[1].id == 1);
    assert(touches[1].x == 512);
    assert(touches[1].y == 768);
    assert(touches[1].pressure == 64);
    
    std::cout << "  ✓ Passed: Correctly parsed 2 simultaneous touch points" << std::endl;
}

// Test 4: M5 Paper S3 resolution bounds check
void test_m5_resolution_bounds() {
    std::cout << "Test 4: M5 Paper S3 resolution bounds..." << std::endl;
    
    GT911Mock gt911;
    
    // Test corner coordinates (600x960 resolution)
    uint8_t raw_data[] = {
        0x82,           // Status: ready + 2 touches
        // Touch at (0, 0)
        0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00,
        // Touch at (599, 959)
        0x01, 0x77, 0x02, 0x7F, 0x03, 0xFF, 0x00, 0x00,
    };
    
    gt911.load_raw_data(raw_data, sizeof(raw_data));
    auto touches = gt911.parse_touch_data();
    
    assert(touches.size() == 2);
    assert(touches[0].x == 0 && touches[0].y == 0);
    assert(touches[1].x == 599 && touches[1].y == 959);
    
    std::cout << "  ✓ Passed: Touch points within M5 Paper S3 bounds (0-599, 0-959)" << std::endl;
}

// Test 5: No valid data (ready bit not set)
void test_no_valid_data() {
    std::cout << "Test 5: Handling invalid data (ready bit not set)..." << std::endl;
    
    GT911Mock gt911;
    
    // Status without ready bit (bit 7 = 0)
    uint8_t raw_data[] = { 0x02, 0, 0, 0, 0 };  // 2 touches but not ready
    
    gt911.load_raw_data(raw_data, sizeof(raw_data));
    assert(!gt911.is_data_ready());
    
    auto touches = gt911.parse_touch_data();
    assert(touches.size() == 0);  // Should return empty despite touch count
    
    std::cout << "  ✓ Passed: Invalid data returns empty touch vector" << std::endl;
}

// Test 6: Input coordinate validation for reading workflow
void test_reading_workflow_coordinates() {
    std::cout << "Test 6: Typical reading workflow coordinates..." << std::endl;
    
    GT911Mock gt911;
    
    // Simulate page-turn gesture: tap on right side of screen
    uint8_t raw_data[] = {
        0x81,           // Status: ready + 1 touch
        // Touch at (520, 480) - right side, middle vertically
        0x00,
        0x08, 0x02,     // X = 0x0208 = 520
        0xE0, 0x01,     // Y = 0x01E0 = 480
        0xAA, 0x00,     // Pressure
        0x00,
    };
    
    gt911.load_raw_data(raw_data, sizeof(raw_data));
    auto touches = gt911.parse_touch_data();
    
    assert(touches.size() == 1);
    assert(touches[0].x == 520);  // Right side (> 300)
    assert(touches[0].y == 480);  // Middle vertically
    
    std::cout << "  ✓ Passed: Typical page-turn tap (520, 480)" << std::endl;
}

// Main test runner
int main() {
    std::cout << "\n=== M5 Paper S3 GT911 Touch Controller Unit Tests ===" << std::endl;
    std::cout << "Testing GT911 capacitive touch input parsing\n" << std::endl;
    
    try {
        test_status_register();
        test_single_touch_parsing();
        test_multiple_touch_parsing();
        test_m5_resolution_bounds();
        test_no_valid_data();
        test_reading_workflow_coordinates();
        
        std::cout << "\n✅ All tests passed!\n" << std::endl;
        return 0;
    } catch (const std::exception & e) {
        std::cerr << "\n❌ Test failed: " << e.what() << std::endl;
        return 1;
    }
}
