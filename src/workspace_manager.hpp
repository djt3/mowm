#ifndef _LAYOUT_MANAGER_H_
#define _LAYOUT_MANAGER_H_

#include <X11/Xlib.h>
#include <array>
#include <vector>
#include <cstdint>

struct window {
  window(Window xwindow, int stack_idx = 0) : xwindow(xwindow), stack_idx(stack_idx) {}

  Window xwindow;
  uint8_t stack_idx;
};

struct workspace {
  std::vector<window> windows;
  int focused_idx, layout_idx;

  void unmap();
  void map();
};

namespace workspace_manager {
  extern std::array<workspace, 10> workspaces;

  extern void focus_window(Window xwindow);
  extern void make_focused_window_primary();
  extern void cycle_window_focus(bool reverse = false);
  extern void cycle_window_stack(bool reverse = false);
  extern void cycle_workspace();
  extern void switch_workspace(int workspace_idx);
  extern void move_focused_window_to_workspace(int workspace_idx);
  extern void kill_focused_window();

  extern void add_new_window(Window xwindow);
  extern void add_window_to_workspace(Window xwindow, int workspace_idx);
  extern void remove_window_from_workspace(Window xwindow, int workspace_idx = -1);

  extern void update_workspace_layout(int workspace_idx);
}

#endif
