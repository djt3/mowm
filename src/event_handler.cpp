#include "event_handler.hpp"
#include "config.hpp"
#include "globals.hpp"
#include "workspace_manager.hpp"
#include "monitor_manager.hpp"

#include <cstring>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>

namespace event_handler {
  namespace events {
    void key_press(const XKeyEvent& event) {
      for (const auto& keybind : config::key_bindings) {
        if (!keybind.triggered_by(event))
          continue;

        // if the keybind isn't for the wm
        if (!keybind.is_wm_command()) {
          if (fork())
            return;
          if (globals::display)
            close(ConnectionNumber(globals::display));

          setsid();
          char* arg[] = {const_cast<char*>(keybind.cmd), NULL};
          execvp(arg[0], arg);
        }

        else if (keybind.wm_command == wm_commands::workspace) {
          int idx = keybind.cmd[strlen(keybind.cmd) - 1] - '0';

          // switch to workspace
          if (!keybind.is_shifted(event))
            workspace_manager::switch_workspace(idx);

          // move window to workspace
          else
            workspace_manager::move_focused_window_to_workspace(idx);
        }

        else if (keybind.wm_command == wm_commands::cycle_win)
          workspace_manager::cycle_window_focus(keybind.cmd[strlen(keybind.cmd) - 1] == 'r');

        else if (keybind.wm_command == wm_commands::cycle_stack)
          workspace_manager::cycle_window_stack(keybind.cmd[strlen(keybind.cmd) - 1] == 'r');

        else if (keybind.wm_command == wm_commands::cycle_workspace)
          workspace_manager::cycle_workspace();

        else if (keybind.wm_command == wm_commands::kill_window)
          workspace_manager::kill_focused_window();

        else if (keybind.wm_command == wm_commands::make_primary)
          workspace_manager::make_focused_window_primary();

        break;
      }
    }

    void map_request(const XMapRequestEvent& event) {
      // get keypress and enter events for new window
      XSelectInput(globals::display, event.window, KeyPressMask | EnterWindowMask);
      // map the new window to the screen
      XMapWindow(globals::display, event.window);

      workspace_manager::add_new_window(event.window);
    }

    void destroy_notify(const XDestroyWindowEvent& event) {
      workspace_manager::remove_window_from_workspace(event.window);
    }

    void enter_notify(const XCrossingEvent& event) {
      workspace_manager::focus_window(event.window);
    }
  }

  void dispatch(XEvent& event) {
    if (event.type == KeyPress)
      events::key_press(event.xkey);
    else if (event.type == MapRequest)
      events::map_request(event.xmaprequest);
    else if (event.type == DestroyNotify)
      events::destroy_notify(event.xdestroywindow);
    else if (event.type == EnterNotify)
      events::enter_notify(event.xcrossing);
    //workspace_manager::update_workspace_layout(monitor_manager::monitors[monitor_manager::active_monitor_idx].workspace_idx);
  }
}
