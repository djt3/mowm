#include "monitor_manager.h"
#include "globals.h"

#include <iostream>

namespace monitor_manager {
  int monitor_count;
  XineramaScreenInfo* screen_info;

  std::vector<monitor> monitors;

  bool init() {
    if (!XineramaIsActive(globals::display)) {
      std::cout << "Error! Xinerama not active!" << std::endl;
      return false;
    }

    // set monitor_count
    get_monitor_count(true);

    for (unsigned i = 0; i < monitor_count; i++) {
      monitor monitor(screen_info[i]);

      if (i >= monitors.size()) {
        monitor.workspace_idx = i;
        monitors.push_back(monitor);
      }
      else {
        monitor.workspace_idx = monitors[i].workspace_idx;
        monitors[i] = monitor;
      }
    }

    return true;
  }

  int get_monitor_count(bool force_update) {
    if (!screen_info || force_update)
      screen_info = XineramaQueryScreens(globals::display, &monitor_count);

    return monitor_count;
  }
}
