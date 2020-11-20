#include "master_stack.hpp"
#include "../workspace_manager.hpp"
#include "../monitor_manager.hpp"
#include "../globals.hpp"

namespace layout_manager {
  void layout_master_stack::update_workspace_layout(int workspace_idx) {
    workspace& workspace = workspace_manager::workspaces[workspace_idx];
    if (workspace.windows.empty())
      return;

    monitor* monitor = monitor_manager::get_monitor_with_workspace(workspace_idx);
    if (!monitor)
      return;

    XSetInputFocus(globals::display, workspace.windows[workspace.focused_idx].xwindow, RevertToPointerRoot, CurrentTime);

    // only one window - fullscreen
    if (workspace.windows.size() == 1)
      XMoveResizeWindow(globals::display, workspace.windows[0].xwindow, monitor->pos.x, monitor->pos.y,
                        monitor->dim.width, monitor->dim.height);


    else {
      auto subheight = monitor->dim.height / (workspace.windows.size() - 1);


      unsigned int y_step = monitor->dim.height / (workspace.windows.size() - 1);

      for (int i = 0; i < workspace.windows.size(); i++) {
        if (workspace.windows[i].stack_idx  == 0)
          XMoveResizeWindow(globals::display, workspace.windows[i].xwindow, monitor->pos.x, monitor->pos.y,
                        monitor->dim.width / 2, monitor->dim.height);
        else
          XMoveResizeWindow(globals::display, workspace.windows[i].xwindow, monitor->pos.x + monitor->dim.width / 2, monitor->pos.y + y_step * (workspace.windows[i].stack_idx - 1),
                           monitor->dim.width / 2, y_step);
      }
    }
  }
}
