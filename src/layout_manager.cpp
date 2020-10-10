#include "layout_manager.h"
#include "monitor_manager.h"
#include "globals.h"

#include <iostream>

namespace layout_manager {
  std::array<workspace, 10> workspaces;

  void workspace::unmap() {
    for (auto& window : windows)
      XUnmapWindow(globals::display, window);
  }

  void workspace::map() {
    for (auto& window : windows)
      XMapWindow(globals::display, window);
  }

  void tab_workspace() {
    switch_workspace(monitor_manager::get_active_monitor().old_workspace_idx);
  }

  void tab_window_focus(bool reverse) {
    int current_workspace_idx = monitor_manager::get_active_monitor().workspace_idx;
    workspace& current_workspace = workspaces[current_workspace_idx];
    if (current_workspace.windows.size() < 2)
      return;

    if (!reverse)
      current_workspace.focused_idx = ++current_workspace.focused_idx % current_workspace.windows.size();
    else {
      current_workspace.focused_idx = --current_workspace.focused_idx;
      if (current_workspace.focused_idx < 0)
        current_workspace.focused_idx = current_workspace.windows.size() - 1;
    }

    XSetInputFocus(globals::display, current_workspace.windows[current_workspace.focused_idx], RevertToPointerRoot, CurrentTime);
  }

  void switch_workspace(int workspace_idx) {
    monitor& current_monitor = monitor_manager::get_active_monitor();
    if (workspace_idx == current_monitor.workspace_idx)
      return;

    // unmap old workspace
    workspaces[current_monitor.workspace_idx].unmap();

    // save previous workspace
    current_monitor.old_workspace_idx = current_monitor.workspace_idx;
    // switch to new workspace
    current_monitor.workspace_idx = workspace_idx;
    workspaces[current_monitor.workspace_idx].map();
    // layout the windows
    update_workspace_layout(workspace_idx);
  }

  void move_focused_window_to_workspace(int workspace_idx) {
    int current_workspace_idx = monitor_manager::get_active_monitor().workspace_idx;
    workspace& current_workspace = workspaces[current_workspace_idx];
    if (current_workspace.windows.empty())
      return;

    Window focused_window = current_workspace.windows[current_workspace.focused_idx];

    remove_window_from_workspace(focused_window, current_workspace_idx);
    add_window_to_workspace(focused_window, workspace_idx);

    // update the original workspace layout
    update_workspace_layout(current_workspace_idx);

    // if the new workspace is visible, update the layout also
    if (monitor_manager::get_monitor_with_workspace(workspace_idx))
      update_workspace_layout(workspace_idx);
    // if not visible, unmap the window
    else
      XUnmapWindow(globals::display, focused_window);
  }

  void kill_focused_window() {
    int current_workspace_idx = monitor_manager::get_active_monitor().workspace_idx;
    workspace& current_workspace = workspaces[current_workspace_idx];
    if (current_workspace.windows.empty())
      return;

    XKillClient(globals::display, current_workspace.windows[current_workspace.focused_idx]);
  }

  void add_new_window(Window window) {
    int workspace_idx = monitor_manager::get_active_monitor().workspace_idx;
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

  void remove_window_from_workspace(Window window, int workspace_idx) {
    auto remove_window = [](const Window& window, int workspace_idx) -> bool {
      workspace& workspace = workspaces[workspace_idx];

      for (int i = 0; i < workspace.windows.size(); i++) {
        if (workspace.windows[i] == window) {
          workspace.windows.erase(workspace.windows.begin() + i);

          if (!workspace.windows.empty()) {
            if (workspace.focused_idx >= workspace.windows.size())
              workspace.focused_idx--;
            if (workspace.primary_idx >= workspace.windows.size())
              workspace.primary_idx--;

            update_workspace_layout(workspace_idx);
          }

          return true;
        }
      }

      return false;
    };

    // if no workspace was specified, find one with this window in
    if (workspace_idx == -1) {
      for (int w = 0; w < workspaces.size(); w++) {
        if (remove_window(window, w))
          break;
      }
    }

    else
      remove_window(window, workspace_idx);
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
