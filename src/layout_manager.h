#ifndef _LAYOUT_MANAGER_H_
#define _LAYOUT_MANAGER_H_

#include <X11/Xlib.h>
#include <array>
#include <vector>

namespace layout_manager {
  struct workspace {
    std::vector<Window> windows;
    int focused_idx, primary_idx;

    void unmap();
    void map();
  };

  extern std::array<workspace, 10> workspaces;

  extern void switch_workspace(int workspace_idx);
  extern void move_focused_window_to_workspace(int workspace_idx);

  extern void add_new_window(Window window);
  extern void add_window_to_workspace(Window window, int workspace_idx);
  extern void remove_window_from_workspace(Window window, int workspace_idx = -1);

  extern void update_workspace_layout(int workspace_idx);
}

#endif
