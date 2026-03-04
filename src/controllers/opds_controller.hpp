// Copyright (c) 2026 OPDS Controller
//
// MIT License. Look at file licenses.txt for details.

#pragma once
#include "global.hpp"

#include "controllers/event_mgr.hpp"

/**
 * @brief OPDS Controller - Manages OPDS catalog browsing interface
 * 
 * Handles online book catalog browsing from booklore.com via OPDS protocol.
 * Provides touch-driven interface for:
 * - Configuration (server URL, username, password)
 * - Catalog browsing and search
 * - Book download
 */
class OPDSController
{
  private:
    static constexpr char const * TAG = "OPDSController";

    bool opds_active;
    bool needs_refresh;

  public:
    OPDSController() : opds_active(false), needs_refresh(true) {};

    /**
     * @brief Initialize OPDS controller
     */
    void setup();

    /**
     * @brief Enter OPDS interface
     */
    void enter();

    /**
     * @brief Leave OPDS interface
     */
    void leave(bool going_to_deep_sleep = false);

    /**
     * @brief Handle input events (touch, buttons, etc)
     */
    void input_event(const EventMgr::Event & event);

    inline bool is_active() { return opds_active; }
};

#if __OPDS_CONTROLLER__
  OPDSController opds_controller;
#else
  extern OPDSController opds_controller;
#endif

