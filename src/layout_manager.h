#ifndef _LAYOUT_MANAGER_H_
#define _LAYOUT_MANAGER_H_

#include <X11/Xlib.h>
#include <array>
#include <vector>

namespace layout_manager {
  struct workspace {
    std::vector<Window> windows;
    int focused_idx, primary_idx;
    bool is_mapped = false;

    void unmap();
    void map();
  };

  extern std::array<workspace, 10> workspaces;

  extern void add_new_window(Window window);
  extern void add_window_to_workspace(Window window, int workspace_id);
  extern void destroy_window(Window window);

  extern void update_workspace_layout(int workspace_idx);
}

#endif
