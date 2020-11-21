#include "monacle.hpp"
#include "../workspace_manager.hpp"
#include "../monitor_manager.hpp"
#include "../globals.hpp"

namespace layout_manager {
  void layout_monacle::update_workspace_layout(int workspace_idx) {
    workspace& workspace = workspace_manager::workspaces[workspace_idx];
    if (workspace.windows.empty())
      return;

    monitor* monitor = monitor_manager::get_monitor_with_workspace(workspace_idx);
    if (!monitor)
      return;

    workspace.focused_idx = 0;
    XSetInputFocus(globals::display, workspace.windows[0].xwindow, RevertToPointerRoot, CurrentTime);

    for (int i = 0; i < workspace.windows.size(); i++) {
      // only one window - fullscreen
      if (workspace.windows[i].stack_idx == 0) {
        XMoveResizeWindow(globals::display, workspace.windows[i].xwindow, monitor->pos.x, monitor->pos.y,
                          monitor->dim.width, monitor->dim.height);

        XMapWindow(globals::display, workspace.windows[i].xwindow);
      }
      else {
        XUnmapWindow(globals::display, workspace.windows[i].xwindow);
      }
    }
  }
}
