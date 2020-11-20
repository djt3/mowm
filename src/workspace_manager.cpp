#include "workspace_manager.hpp"
#include "monitor_manager.hpp"
#include "globals.hpp"
#include "layouts/layouts.hpp"

#include <iostream>

void workspace::unmap() {
  for (auto& window : windows)
    XUnmapWindow(globals::display, window.xwindow);
}

void workspace::map() {
  for (auto& window : windows)
    XMapWindow(globals::display, window.xwindow);
}

namespace workspace_manager {
  std::array<workspace, 10> workspaces;


  int find_window_idx_in_workspace(Window xwindow, int workspace_idx) {
    workspace& workspace = workspaces[workspace_idx];

    for (int i = 0; i < workspace.windows.size(); i++) {
      if (workspace.windows[i].xwindow == xwindow)
        return i;
    }

    return -1;
  }

  int find_workspace_idx_with_window(Window xwindow) {
    for (int w = 0; w < workspaces.size(); w++) {
      const workspace& workspace = workspaces[w];

      for (int i = 0; i < workspace.windows.size(); i++) {
        if (xwindow == workspace.windows[i].xwindow)
          return w;
      }
    }

    return -1;
  }

  void focus_window(Window xwindow) {
    int workspace_idx = find_workspace_idx_with_window(xwindow);
    if (workspace_idx == -1)
      return;

    workspace& workspace = workspaces[workspace_idx];

    int window_idx = find_window_idx_in_workspace(xwindow, workspace_idx);
    if (window_idx == -1)
      return;

    XSetInputFocus(globals::display, xwindow, RevertToPointerRoot, CurrentTime);

    workspace.focused_idx = window_idx;
  }

  void make_focused_window_primary() {
    int current_workspace_idx = monitor_manager::get_active_monitor().workspace_idx;
    workspace& current_workspace = workspaces[current_workspace_idx];
    if (current_workspace.windows.size() < 2)
      return;

    auto& focused_window = current_workspace.windows[current_workspace.focused_idx];

    // find the current primary window and swap its position with the curent focused window
    for (auto& window : current_workspace.windows) {
      if (window.stack_idx == 0)
        window.stack_idx = focused_window.stack_idx;
    }

    focused_window.stack_idx = 0;

    update_workspace_layout(current_workspace_idx);
  }

  void cycle_workspace() {
    switch_workspace(monitor_manager::get_active_monitor().old_workspace_idx);
  }

  void cycle_window_focus(bool reverse) {
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

    XSetInputFocus(globals::display, current_workspace.windows[current_workspace.focused_idx].xwindow, RevertToPointerRoot, CurrentTime);
  }

  void cycle_window_stack(bool reverse) {
    int current_workspace_idx = monitor_manager::get_active_monitor().workspace_idx;
    workspace& current_workspace = workspaces[current_workspace_idx];
    if (current_workspace.windows.size() < 2)
      return;

    auto max_idx = current_workspace.windows.size() - 1;

    for (auto& window : current_workspace.windows) {
      if (reverse && window.stack_idx == 0)
        window.stack_idx = max_idx; 
      else if (!reverse && window.stack_idx == max_idx)
        window.stack_idx = 0;
      
      else if (reverse)
       window.stack_idx--; 
      else
        window.stack_idx++;
    }

    update_workspace_layout(current_workspace_idx);
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

    Window focused_window = current_workspace.windows[current_workspace.focused_idx].xwindow;

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

    XSetCloseDownMode(globals::display, DestroyAll);
    XKillClient(globals::display, current_workspace.windows[current_workspace.focused_idx].xwindow);
  }

  void add_new_window(Window xwindow) {
    int workspace_idx = monitor_manager::get_active_monitor().workspace_idx;
    add_window_to_workspace(xwindow, workspace_idx);
  }

  void add_window_to_workspace(Window xwindow, int workspace_idx) {
    workspace& workspace = workspaces[workspace_idx];

    // focus the new window
    int new_window_idx = workspace.windows.size();
    workspace.focused_idx = new_window_idx;

    for (auto& window : workspace.windows)
      window.stack_idx++;

    window window(xwindow);

    workspace.windows.push_back(window);

    // if the workspace is visible
    if (monitor_manager::get_monitor_with_workspace(workspace_idx))
      update_workspace_layout(workspace_idx);
  }

  void remove_window_from_workspace(Window xwindow, int workspace_idx) {
    auto remove_window = [](const Window& xwindow, int workspace_idx) -> void {
      workspace& workspace = workspaces[workspace_idx];

      for (int i = 0; i < workspace.windows.size(); i++) {
        if (workspace.windows[i].xwindow == xwindow) {
          uint8_t idx = workspace.windows[i].stack_idx;

          workspace.windows.erase(workspace.windows.begin() + i);

          if (!workspace.windows.empty()) {
            if (workspace.focused_idx >= workspace.windows.size())
              workspace.focused_idx--;

            for (auto& window : workspace.windows) {
              if (window.stack_idx > idx)
                window.stack_idx--;
            }

            update_workspace_layout(workspace_idx);
          }

          return;
        }
      }
    };

    // if no workspace was specified, find one with this window in
    if (workspace_idx == -1)
      remove_window(xwindow, find_workspace_idx_with_window(xwindow));

    else
      remove_window(xwindow, workspace_idx);
  }

  void update_workspace_layout(int workspace_idx) {
    layout_manager::layouts[layout_manager::current_layout]->update_workspace_layout(workspace_idx);
  }
}
