// Copyright (c) 2026 OPDS Controller
//
// MIT License. Look at file licenses.txt for details.

#define __OPDS_CONTROLLER__ 1
#include "controllers/opds_controller.hpp"

#include "controllers/app_controller.hpp"
#include "controllers/event_mgr.hpp"
#include "models/config.hpp"

#if M5_PAPER_S3
  #include "helpers/opds_display_adapter.hpp"
  #include "services/opds_ui_manager.hpp"
#endif

// Global OPDS display panel adapter
#if M5_PAPER_S3
  static M5EPaperPanel g_opds_panel;
#endif

void
OPDSController::setup()
{
  LOG_D("OPDS Controller setup");

  #if M5_PAPER_S3
    // Initialize OPDS UI manager with display adapter
    if (g_opds_ui_manager) {
      if (g_opds_ui_manager->init(&g_opds_panel)) {
        LOG_I("OPDS UI Manager initialized");
      } else {
        LOG_E("Failed to initialize OPDS UI Manager");
      }
    }
  #endif

  opds_active = false;
}

void
OPDSController::enter()
{
  LOG_I("Entering OPDS interface");
  opds_active = true;
  needs_refresh = true;

  #if M5_PAPER_S3
    if (g_opds_ui_manager) {
      g_opds_ui_manager->render();
    }
  #endif
}

void
OPDSController::leave(bool going_to_deep_sleep)
{
  LOG_I("Leaving OPDS interface");
  opds_active = false;
}

void
OPDSController::input_event(const EventMgr::Event & event)
{
  if (!opds_active) {
    return;
  }

  #if M5_PAPER_S3
    if (!g_opds_ui_manager) {
      return;
    }

    switch (event.kind) {
      case EventMgr::EventKind::TAP:
        // Convert TAP to touch press event
        LOG_D("OPDS: TAP at (%d, %d)", event.x, event.y);
        g_opds_ui_manager->on_touch_event(event.x, event.y, true);
        // Re-render after touch handling
        g_opds_ui_manager->render();
        break;

      case EventMgr::EventKind::SWIPE_LEFT:
        // Previous page in book list or navigate back
        LOG_D("OPDS: SWIPE_LEFT (navigate)");
        // Can be handled by OPDS UI state machine
        break;

      case EventMgr::EventKind::SWIPE_RIGHT:
        // Next page in book list or go back to previous screen
        LOG_D("OPDS: SWIPE_RIGHT (navigate)");
        
        // For now, use swipe right as "back" to return to books directory
        if (g_opds_ui_manager->get_state() == OPDS_STATE_MENU) {
          // If already at main menu, go back to books directory
          app_controller.set_controller(AppController::Ctrl::DIR);
        }
        break;

      case EventMgr::EventKind::RELEASE:
        // Touch release
        LOG_D("OPDS: RELEASE at (%d, %d)", event.x, event.y);
        g_opds_ui_manager->on_touch_event(event.x, event.y, false);
        break;

      case EventMgr::EventKind::HOLD:
        // Long press - reserved for future use
        LOG_D("OPDS: HOLD at (%d, %d)", event.x, event.y);
        break;

      default:
        break;
    }
  #endif
}

