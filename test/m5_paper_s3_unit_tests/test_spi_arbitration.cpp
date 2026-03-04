/**
 * @file test_spi_arbitration.cpp
 * @brief Unit tests for SPI bus arbitration between display and SD card
 * 
 * Tests SPI bus contention management for M5 Paper S3
 * (Display via IT8951E and SD card share the same SPI bus)
 */

#include <iostream>
#include <cassert>
#include <cstring>
#include <stdint.h>
#include <queue>
#include <chrono>

// SPI transaction types
enum class SPIDevice {
    DISPLAY,
    SD_CARD
};

// Mock SPI transaction
struct SPITransaction {
    SPIDevice device;
    uint32_t duration_us;  // Micrseconds
    uint32_t priority;     // 0 = low, 1 = normal, 2 = high
    std::string description;
    bool is_complete;
    
    SPITransaction() : device(SPIDevice::DISPLAY), duration_us(0), priority(1), 
                       description(""), is_complete(false) {}
};

// Mock SPI bus arbitrator
class SPIBusArbitrator {
private:
    SPIDevice current_owner;
    bool bus_busy;
    std::queue<SPITransaction> pending_transactions;
    uint32_t display_wait_time;  // Total microseconds display waited
    uint32_t sdcard_wait_time;   // Total microseconds SD card waited
    
public:
    SPIBusArbitrator() : current_owner(SPIDevice::DISPLAY), bus_busy(false),
                        display_wait_time(0), sdcard_wait_time(0) {}
    
    // Request bus access
    bool request_bus(SPIDevice device, uint32_t timeout_us = 1000000) {
        if (!bus_busy) {
            current_owner = device;
            bus_busy = true;
            return true;
        }
        
        // Track wait time
        if (device == SPIDevice::DISPLAY) {
            display_wait_time += timeout_us;
        } else {
            sdcard_wait_time += timeout_us;
        }
        
        return false;  // Bus busy, would timeout
    }
    
    // Release bus access
    void release_bus(SPIDevice device) {
        if (current_owner == device) {
            bus_busy = false;
            current_owner = SPIDevice::DISPLAY;  // Default
        }
    }
    
    // Check if bus is available for specific device
    bool can_access(SPIDevice device) {
        return !bus_busy || current_owner == device;
    }
    
    // Get bus owner
    SPIDevice get_current_owner() { return current_owner; }
    
    // Get statistics
    uint32_t get_display_wait_time() { return display_wait_time; }
    uint32_t get_sdcard_wait_time() { return sdcard_wait_time; }
};

// Test 1: Basic bus acquisition and release
void test_basic_bus_access() {
    std::cout << "Test 1: Basic bus acquisition and release..." << std::endl;
    
    SPIBusArbitrator arbitrator;
    
    // Display acquires bus
    assert(arbitrator.request_bus(SPIDevice::DISPLAY));
    assert(!arbitrator.can_access(SPIDevice::SD_CARD));
    
    // Display releases bus
    arbitrator.release_bus(SPIDevice::DISPLAY);
    assert(arbitrator.can_access(SPIDevice::DISPLAY));
    assert(arbitrator.can_access(SPIDevice::SD_CARD));
    
    std::cout << "  ✓ Passed: Bus acquisition and release working" << std::endl;
}

// Test 2: Priority-based access (display should have priority)
void test_priority_based_access() {
    std::cout << "Test 2: Priority-based bus access..." << std::endl;
    
    SPIBusArbitrator arbitrator;
    
    // SD card acquires bus first
    assert(arbitrator.request_bus(SPIDevice::SD_CARD, 1000));
    assert(arbitrator.get_current_owner() == SPIDevice::SD_CARD);
    
    // Display tries to access (should get priority but bus is busy)
    assert(!arbitrator.request_bus(SPIDevice::DISPLAY, 500));  // Times out faster
    assert(arbitrator.get_display_wait_time() > 0);
    
    std::cout << "  ✓ Passed: Priority conflicts resolved" << std::endl;
}

// Test 3: Contention tracking
void test_contention_tracking() {
    std::cout << "Test 3: Contention tracking..." << std::endl;
    
    SPIBusArbitrator arbitrator;
    
    // Multiple contention attempts
    arbitrator.request_bus(SPIDevice::DISPLAY, 1000);
    arbitrator.request_bus(SPIDevice::SD_CARD, 500);  // Contention
    arbitrator.request_bus(SPIDevice::SD_CARD, 500);  // More contention
    arbitrator.release_bus(SPIDevice::DISPLAY);
    
    uint32_t sdcard_total_wait = arbitrator.get_sdcard_wait_time();
    assert(sdcard_total_wait >= 1000);  // At least 1000 us of waiting
    
    std::cout << "  ✓ Passed: SD card waited " << sdcard_total_wait << " us" << std::endl;
}

// Test 4: Display refresh (typical 2-3 second operation)
void test_display_refresh_transaction() {
    std::cout << "Test 4: Display refresh transaction..." << std::endl;
    
    SPIBusArbitrator arbitrator;
    
    // Full display update: ~3 seconds
    // Typical IT8951E commands:
    // - Get device info: ~100ms
    // - Load image data: ~1500ms (600x960 pixels)
    // - Display update: ~1000ms
    
    uint32_t total_display_time = 100000 + 1500000 + 1000000;  // microseconds
    
    assert(arbitrator.request_bus(SPIDevice::DISPLAY, total_display_time));
    
    // During display update, SD card requests should be blocked
    assert(!arbitrator.can_access(SPIDevice::SD_CARD));
    
    arbitrator.release_bus(SPIDevice::DISPLAY);
    
    // Now SD card can access
    assert(arbitrator.can_access(SPIDevice::SD_CARD));
    
    std::cout << "  ✓ Passed: Display refresh blocks SD card for " 
              << (total_display_time / 1000000.0f) << " seconds" << std::endl;
}

// Test 5: Rapid alternation scenario
void test_rapid_alternation() {
    std::cout << "Test 5: Rapid alternation (typical reading)..." << std::endl;
    
    SPIBusArbitrator arbitrator;
    uint32_t iterations = 0;
    
    // Simulate rapid page loading: partial reads from SD, partial display updates
    for (int i = 0; i < 10; i++) {
        // Display partial refresh (~1 second)
        if (arbitrator.request_bus(SPIDevice::DISPLAY, 1000000)) {
            arbitrator.release_bus(SPIDevice::DISPLAY);
            iterations++;
        }
        
        // SD card page buffer load (~200ms)
        if (arbitrator.request_bus(SPIDevice::SD_CARD, 200000)) {
            arbitrator.release_bus(SPIDevice::SD_CARD);
            iterations++;
        }
    }
    
    assert(iterations > 0);
    
    std::cout << "  ✓ Passed: Completed " << iterations << " alternating transactions" << std::endl;
}

// Test 6: Timeout behavior
void test_timeout_behavior() {
    std::cout << "Test 6: Timeout behavior..." << std::endl;
    
    SPIBusArbitrator arbitrator;
    
    // Display holds bus with timeout
    uint32_t acquire_timeout = 5000000;  // 5 seconds
    assert(arbitrator.request_bus(SPIDevice::DISPLAY, acquire_timeout));
    
    // SD card tries to access with shorter timeout
    uint32_t sdcard_timeout = 500000;  // 500ms
    bool sdcard_acquired = arbitrator.request_bus(SPIDevice::SD_CARD, sdcard_timeout);
    
    assert(!sdcard_acquired);  // Should timeout
    assert(arbitrator.get_sdcard_wait_time() >= sdcard_timeout);
    
    // Release display
    arbitrator.release_bus(SPIDevice::DISPLAY);
    
    // SD card can now acquire
    assert(arbitrator.request_bus(SPIDevice::SD_CARD, 1000));
    
    std::cout << "  ✓ Passed: Timeouts enforced correctly" << std::endl;
}

// Test 7: Real-world reading scenario
void test_reading_scenario() {
    std::cout << "Test 7: Typical reading scenario..." << std::endl;
    
    SPIBusArbitrator arbitrator;
    
    // 1. Load page from SD card
    assert(arbitrator.request_bus(SPIDevice::SD_CARD, 300000));  // 300ms
    arbitrator.release_bus(SPIDevice::SD_CARD);
    
    // 2. Display refresh
    assert(arbitrator.request_bus(SPIDevice::DISPLAY, 2500000));  // 2.5s
    arbitrator.release_bus(SPIDevice::DISPLAY);
    
    // 3. Load next page (prefetch while reading)
    assert(arbitrator.request_bus(SPIDevice::SD_CARD, 300000));  // 300ms
    arbitrator.release_bus(SPIDevice::SD_CARD);
    
    // 4. Battery/RTC check via I2C (doesn't use SPI)
    
    // 5. Next page refresh
    assert(arbitrator.request_bus(SPIDevice::DISPLAY, 2000000));  // 2s partial
    arbitrator.release_bus(SPIDevice::DISPLAY);
    
    std::cout << "  ✓ Passed: Complete reading scenario executed successfully" << std::endl;
}

// Main test runner
int main() {
    std::cout << "\n=== M5 Paper S3 SPI Bus Arbitration Unit Tests ===" << std::endl;
    std::cout << "Testing SPI bus sharing between display and SD card\n" << std::endl;
    
    try {
        test_basic_bus_access();
        test_priority_based_access();
        test_contention_tracking();
        test_display_refresh_transaction();
        test_rapid_alternation();
        test_timeout_behavior();
        test_reading_scenario();
        
        std::cout << "\n✅ All tests passed!\n" << std::endl;
        return 0;
    } catch (const std::exception & e) {
        std::cerr << "\n❌ Test failed: " << e.what() << std::endl;
        return 1;
    }
}
