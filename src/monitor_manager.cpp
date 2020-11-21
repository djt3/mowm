#include "monitor_manager.hpp"
#include "globals.hpp"

#include <iostream>

namespace monitor_manager {
  int monitor_count;
  XineramaScreenInfo* screen_info;

  std::vector<monitor> monitors;
  int active_monitor_idx = 0;

  void init() {
    if (!XineramaIsActive(globals::display)) {
      std::cout << "Error! Xinerama not active!" << std::endl;
      return;
    }

    // set monitor_count
    int old_monitor_count = monitor_count;
    int monitor_count = get_monitor_count(true);

    monitors.resize(monitor_count);

    for (unsigned i = 0; i < monitor_count; i++) {
      monitor monitor(screen_info[i]);

      if (i >= old_monitor_count) {
        monitor.workspace_idx = i + 1;
        monitor.old_workspace_idx = i + 1;
        monitors[i] = monitor;
      }
      else {
        monitor.workspace_idx = monitors[i].workspace_idx;
        monitor.old_workspace_idx = monitors[i].old_workspace_idx;
      }
        
      monitors[i] = monitor;
    }

    return;
  }

  int get_monitor_count(bool force_update) {
    //if (!screen_info || force_update)
      screen_info = XineramaQueryScreens(globals::display, &monitor_count);

    return monitor_count;
  }

  monitor& get_active_monitor() {
    return monitors[active_monitor_idx];
  }

  int get_monitor_idx_with_workspace(int workspace_idx) {
    for (int i = 0; i < monitors.size(); i++) {
      if (monitors[i].workspace_idx == workspace_idx)
        return i;
    }
    return -1;
  }

  monitor* get_monitor_with_workspace(int workspace_idx) {
    int idx = get_monitor_idx_with_workspace(workspace_idx);
    if (idx != -1)
      return &monitors[idx];

    return nullptr;
  }

  void focus_monitor_with_workspace(int workspace_idx) {
    int idx = get_monitor_idx_with_workspace(workspace_idx);
    if (idx == -1)
      return;

    active_monitor_idx = idx;
  }
}
