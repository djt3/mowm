#include "layout_manager.h"
#include "monitor_manager.h"
#include "globals.h"

#include <iostream>

namespace layout_manager {
  std::array<workspace, 10> workspaces;

  void workspace::unmap() {
    if (!is_mapped)
      return;

    for (auto& window : windows)
      XUnmapWindow(globals::display, window);

    is_mapped = false;
  }

  void workspace::map() {
    if (is_mapped)
      return;

    for (auto& window : windows)
      XMapWindow(globals::display, window);

    is_mapped = true;
  }

  void switch_workspace(int workspace_idx) {
    auto& current_monitor = monitor_manager::monitors[monitor_manager::active_monitor_idx];
    // unmap old workspace
    workspaces[current_monitor.workspace_idx].unmap();
    // map new workspace
    current_monitor.workspace_idx = workspace_idx;
    workspaces[current_monitor.workspace_idx].map();
  }

  void add_new_window(Window window) {
    int workspace_idx = monitor_manager::monitors[monitor_manager::active_monitor_idx].workspace_idx;
    add_window_to_workspace(window, workspace_idx);
  }

  void add_window_to_workspace(Window window, int workspace_idx) {
    workspace& workspace = workspaces[workspace_idx];

    // focus the new window
    int new_window_idx = workspace.windows.size();
    workspace.focused_idx = new_window_idx;
    workspace.primary_idx = new_window_idx;

    workspace.windows.push_back(window);

    // if the workspace is visible
    if (monitor_manager::get_monitor_with_workspace(workspace_idx))
      update_workspace_layout(workspace_idx);
  }

  void destroy_window(Window window) {
    for (int w = 0; w < workspaces.size(); w++) {
      workspace& workspace = workspaces[w];

      for (int i = 0; i < workspace.windows.size(); i++) {
        if (workspace.windows[i] == window) {
          workspace.windows.erase(workspace.windows.begin() + i);

          if (!workspace.windows.empty()) {
            if (workspace.focused_idx >= workspace.windows.size())
              workspace.focused_idx--;
            if (workspace.primary_idx >= workspace.windows.size())
              workspace.primary_idx--;

            update_workspace_layout(w);
          }

          w = 1000;
          break;
        }
      }
    }
  }

  void update_workspace_layout(int workspace_idx) {
    workspace& workspace = workspaces[workspace_idx];
    if (workspace.windows.empty())
      return;

    monitor* monitor = monitor_manager::get_monitor_with_workspace(workspace_idx);
    if (!monitor)
      return;

    XSetInputFocus(globals::display, workspace.windows[workspace.focused_idx], RevertToPointerRoot, CurrentTime);

    // only one window - fullscreen
    if (workspace.windows.size() == 1)
      XMoveResizeWindow(globals::display, workspace.windows[0], monitor->pos.x, monitor->pos.y,
                        monitor->dim.width, monitor->dim.height);


    else {
      auto subheight = monitor->dim.height / (workspace.windows.size() - 1);


      std::cout << workspace.windows[workspace.primary_idx] << "primary" << std::endl;
      // primary window takes up the left
      XMoveResizeWindow(globals::display, workspace.windows[workspace.primary_idx], monitor->pos.x, monitor->pos.y,
                        monitor->dim.width / 2, monitor->dim.height);

      unsigned int current_y = 0;
      unsigned int y_step = monitor->dim.height / (workspace.windows.size() - 1);

      for (int i = 0; i < workspace.windows.size(); i++) {
        if (i == workspace.primary_idx)
          continue;

        XMoveResizeWindow(globals::display, workspace.windows[i], monitor->pos.x + monitor->dim.width / 2, monitor->pos.y + current_y,
                          monitor->dim.width / 2, y_step);

        current_y += y_step;
      }
    }
  }
}
