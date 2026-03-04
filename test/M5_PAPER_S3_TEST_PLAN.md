# Phase 5: M5 Paper S3 Testing & Validation Plan

**Status**: IN PROGRESS  
**Created**: March 3, 2026  
**Target Completion**: Week 8  

---

## Overview

This document outlines the comprehensive testing strategy for M5 Paper S3 support in the EPub-InkPlate reader. Testing is organized into three tiers:
1. **Unit Tests** - Individual component testing (non-hardware dependent)
2. **Integration Tests** - Multi-component interaction testing
3. **Hardware Tests** - Device-specific validations on real M5 Paper S3

---

## Unit Tests (Tier 1)

### ✓ Completed Unit Tests

#### 1.1 Frame Buffer 2-Bit Conversion (`test_framebuffer_2bit.cpp`)
**Purpose**: Validate 2-bit grayscale frame buffer implementation for M5 Paper S3

**Tests**:
- ✓ Pixel setting on 2-bit buffers
- ✓ Memory layout validation (600×960 = 144KB)
- ✓ 1-bit to 2-bit conversion (monochrome to grayscale)
- ✓ Byte boundary handling (4 pixels/byte)
- ✓ Large buffer performance (full resolution)

**Status**: **PASSED** ✅

---

#### 1.2 GT911 Touch Controller (`test_gt911_touch.cpp`)
**Purpose**: Validate GT911 capacitive touch input parsing

**Tests**:
- ✓ Status register interpretation (ready bit, touch count)
- ✓ Single touch point parsing
- ✓ Multiple touch points (max 2 simultaneous)
- ✓ M5 Paper S3 resolution bounds (0-599, 0-959)
- ✓ Invalid data handling (missing ready bit)
- ✓ Typical reading workflow coordinates

**Status**: **PASSED** ✅

---

#### 1.3 SPI Bus Arbitration (`test_spi_arbitration.cpp`)
**Purpose**: Validate SPI bus sharing between IT8951E display and SD card

**Tests**:
- ✓ Basic bus acquisition and release
- ✓ Priority-based access (display > SD card)
- ✓ Contention tracking
- ✓ Display refresh transaction (2-3 seconds)
- ✓ Rapid alternation scenario
- ✓ Timeout behavior
- ✓ Real-world reading scenario

**Status**: **PASSED** ✅

---

### Pending Unit Tests

#### 1.4 IT8951E Register Read/Write
**File**: `test_it8951_registers.cpp` (TO CREATE)

**Purpose**: Validate IT8951E display controller command parsing

**Tests**:
- Command 0x6000 (Get device info)
- Command 0x3000 (System register operations)
- Command 0x4000 (Load image with LUT)
- Command 0x5000 (Display area)
- Command 0x2000 (Power on/off)
- Command 0x0000 (Wait for ready)

**Target**: Complete by Day 2

---

#### 1.5 Power Management
**File**: `test_power_management.cpp` (TO CREATE)

**Purpose**: Validate battery monitoring and power transitions

**Tests**:
- Battery voltage reading (0.0V to 4.2V range)
- Battery percentage calculation
- Charge detection
- Deep sleep entry/exit
- Wakeup from various sources

**Target**: Complete by Day 2

---

## Integration Tests (Tier 2)

### 2.1 Full Page Render & Display
**File**: `test_page_rendering.esp32s3` (TO CREATE)

**Scope**: Device-specific integration test  
**Hardware Required**: M5 Paper S3

**Test Steps**:
1. Load test EPUB file from SD card
2. Parse and render page to frame buffer
3. Convert 24-bit color to 2-bit grayscale
4. Send to display via IT8951E
5. Verify display updates within timeout

**Success Criteria**:
- Display updates within 3 seconds
- No visual artifacts
- Text readable at all font sizes
- Images displayed with acceptable dithering

---

### 2.2 Touch Navigation
**File**: `test_touch_navigation.esp32s3` (TO CREATE)

**Scope**: Device-specific integration test  
**Hardware Required**: M5 Paper S3

**Test Steps**:
1. Display menu on screen
2. Tap different menu items
3. Verify correct item selection
4. Test page turning gestures
5. Test TOC navigation

**Success Criteria**:
- Touch coordinates correctly mapped to UI
- No missed touches
- No false positives
- Response time < 100ms

---

### 2.3 Power Management Transitions
**File**: `test_power_transitions.esp32s3` (TO CREATE)

**Scope**: Device-specific integration test  
**Hardware Required**: M5 Paper S3 + power meter

**Test Steps**:
1. Device in reading state
2. Press wakeup button → sleep
3. Measure power consumption in sleep
4. Return to reading state
5. Repeat deep sleep cycle 10 times

**Success Criteria**:
- Sleep entry < 500ms
- Sleep current < 50µA
- Wake-up time < 1 second
- No corruption of display state

---

### 2.4 SD Card Concurrent Operations
**File**: `test_sdcard_display_concurrent.esp32s3` (TO CREATE)

**Scope**: Device-specific integration test  
**Hardware Required**: M5 Paper S3 + microSD card

**Test Steps**:
1. Begin page loading from SD card (large EPUB)
2. Simultaneously trigger display refresh
3. Load next page while displaying current
4. Verify no data corruption
5. Check final page content accuracy

**Success Criteria**:
- No SPI arbitration errors
- Both operations complete successfully
- No file corruption
- Display quality maintained

---

## Hardware Tests (Tier 3)

### 3.1 Boot & Initialization
**Test**: Device power-on behavior

```
Checklist:
☐ Device detects M5 Paper S3 on startup
☐ ESP32-S3 boot messages visible in serial monitor
☐ Display shows splash screen
☐ Touch screen responds to tap
☐ Battery level displayed
☐ SD card initialization successful
```

---

### 3.2 Display Rendering
**Test**: Visual quality verification

```
Checklist:
☐ Text renders clearly at all font sizes
☐ Black-text-on-white readable
☐ Grayscale gradient dithering acceptable
☐ No visible artifacts or flicker
☐ Partial updates don't disturb rest of display
☐ Full refresh clears previous content completely
```

---

### 3.3 Touch Input
**Test**: Touch responsiveness and accuracy

```
Checklist:
☐ Single tap detection working
☐ Double tap detected (if implemented)
☐ Swipe left/right recognized
☐ Swipe up/down recognized
☐ Touch coordinates accurate across screen
☐ No ghost touches on idle screen
☐ Calibration mode accessible
```

---

### 3.4 Book Reading Workflow
**Test**: End-to-end reading experience

```
Checklist:
☐ Open EPUB from SD card
☐ Page renders correctly
☐ Text selectable
☐ Navigate with touch (next/prev page)
☐ TOC navigation works
☐ Bookmarks functional
☐ Reading progress saved
☐ Return to saved position
☐ Font size adjustments work
☐ Margin/line spacing options functional
```

---

### 3.5 Power Management
**Test**: Battery behavior and sleep modes

```
Checklist:
☐ Battery level displays accurately
☐ Charging indicator shows when connected
☐ Device sleeps after timeout
☐ Wake from sleep responsive
☐ Display state preserved after sleep
☐ Deep sleep consumes < 50µA
```

---

### 3.6 microSD Integration
**Test**: File I/O and storage

```
Checklist:
☐ Read EPUB files from SD card
☐ Font loading from SD card
☐ Configuration persistence
☐ Large files (> 10MB) load successfully
☐ Concurrent display + SD operations stable
☐ Hot-swap SD card handled gracefully
```

---

### 3.7 Performance Benchmarks
**Test**: Speed and efficiency measurements

```
Measurements (typical values):
☐ Page turn latency: < 2 seconds
☐ Display refresh: 1.5-2.5 seconds (full)
☐ Touch response: < 100ms
☐ SD read speed: > 10 MB/s
☐ Memory usage: < 4 MB (SRAM + PSRAM)
☐ CPU load during page turn: < 80%
```

---

### 3.8 Stress & Stability Tests
**Test**: Extended operation stability

```
Checklist:
☐ **24-hour uptime test**: Continuous display updates, page turns every 30s
☐ **Temperature cycling**: 0°C to 50°C, 5 cycles
☐ **Memory leaks**: Monitor heap fragmentation over 2 hours
☐ **Power cycles**: 20 on/off cycles without corruption
☐ **SD card stress**: 1000 file reads without errors
```

---

## Test Execution Schedule

| Phase | Duration | Tests | Status |
|-------|----------|-------|--------|
| **Unit Tests** | Days 1-2 | 1.1-1.3 (7 tests) | ✅ **COMPLETE** |
| **IT8951 & Power** | Days 2-3 | 1.4-1.5 (12 tests) | ⏳ In Progress |
| **Integration** | Days 4-5 | 2.1-2.4 (4 suites) | 📅 Scheduled |
| **Device Validation** | Days 6-8 | 3.1-3.8 (8 categories) | 📅 Scheduled |
| **Final Review** | Day 8 | Regression tests | 📅 Scheduled |

---

## Test Infrastructure

### Unit Test Build
```bash
# Compile unit tests (no device required)
cd test/m5_paper_s3_unit_tests
g++ -std=c++17 -O2 -o test_framebuffer test_framebuffer_2bit.cpp
g++ -std=c++17 -O2 -o test_touch test_gt911_touch.cpp
g++ -std=c++17 -O2 -o test_spi test_spi_arbitration.cpp

# Run all unit tests
./test_framebuffer
./test_touch
./test_spi
```

### Integration Test Build
```bash
# Compile for ESP32-S3
export M5_PAPER_S3=1
idf.py build -DDEVICE=M5_PAPER_S3 -DAPP_VERSION=2.2.0

# Flash to device
idf.py flash

# Monitor output
idf.py monitor
```

---

## Known Issues & Mitigations

| Issue | Severity | Status | Mitigation |
|-------|----------|--------|------------|
| IT8951E timing sensitivity | High | 🔍 Under investigation | Use external logic analyzer for SPI debugging |
| Touch calibration variance | Medium | 📋 Identified | Implement per-device calibration routine |
| SPI bus contention spikes | Medium | 🔧 Fixed | Implemented priority queue with timeouts |
| Memory fragmentation under load | Low | 📊 Monitoring | Optimize buffer allocation patterns |

---

## Success Criteria

✅ **All unit tests pass**  
✅ **All integration tests pass**  
✅ **Device boots and initializes correctly**  
✅ **EPUB files render readable on display**  
✅ **Touch navigation smooth and responsive**  
✅ **Battery lasts ≥ 14 days (1 hr/day reading)**  
✅ **No compilation warnings**  
✅ **Performance comparable to InkPlate 6PLUS_V2**  

---

## Next Steps

1. **Today**: Complete unit tests 1.4-1.5
2. **Tomorrow**: Begin integration test development
3. **Next**: Prepare M5 Paper S3 device for hardware testing
4. **Final**: Execute full test matrix and document results

---

**Test Plan Owner**: Development Team  
**Review Date**: March 8, 2026  
**Version**: 1.0
