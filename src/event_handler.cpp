#include "event_handler.h"
#include "config.h"
#include "globals.h"
#include "layout_manager.h"
#include "monitor_manager.h"

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
            layout_manager::switch_workspace(idx);

          // move window to workspace
          else
            layout_manager::move_focused_window_to_workspace(idx);
        }

        else if (keybind.wm_command == wm_commands::cycle_win)
          layout_manager::cycle_window_focus(keybind.cmd[strlen(keybind.cmd) - 1] == 'r');

        else if (keybind.wm_command == wm_commands::cycle_stack)
          layout_manager::cycle_window_stack(keybind.cmd[strlen(keybind.cmd) - 1] == 'r');

        else if (keybind.wm_command == wm_commands::cycle_workspace)
          layout_manager::cycle_workspace();

        else if (keybind.wm_command == wm_commands::kill_window)
          layout_manager::kill_focused_window();

        else if (keybind.wm_command == wm_commands::make_primary)
          layout_manager::make_focused_window_primary();

        break;
      }
    }

    void map_request(const XMapRequestEvent& event) {
      // get keypress and enter events for new window
      XSelectInput(globals::display, event.window, KeyPressMask | EnterWindowMask);
      // map the new window to the screen
      XMapWindow(globals::display, event.window);

      layout_manager::add_new_window(event.window);
    }

    void destroy_notify(const XDestroyWindowEvent& event) {
      layout_manager::remove_window_from_workspace(event.window);
    }

    void enter_notify(const XCrossingEvent& event) {
      layout_manager::focus_window(event.window);
    }
  }

  void dispatch(XEvent& event) {
    if (event.type == DestroyNotify)
      std::cout << "destroy" << std::endl;
    else if (event.type == ConfigureRequest)
      std::cout << "confreq" << std::endl;
    else if (event.type == FocusIn)
      std::cout << "focin" << std::endl;
    else if (event.type == FocusOut)
      std::cout << "focout" << std::endl;
    else if (event.type == MapRequest)
      std::cout << "mapreq" << std::endl;
    else if (event.type == MapNotify)
      std::cout << "mapnotif" << std::endl;
    else if (event.type == UnmapNotify)
      std::cout << "unmapnotif" << std::endl;
    else if (event.type > 3)
      std::cout << event.type << std::endl;

    if (event.type == KeyPress)
      events::key_press(event.xkey);
    else if (event.type == MapRequest)
      events::map_request(event.xmaprequest);
    else if (event.type == DestroyNotify)
      events::destroy_notify(event.xdestroywindow);
    else if (event.type == EnterNotify)
      events::enter_notify(event.xcrossing);
    //layout_manager::update_workspace_layout(monitor_manager::monitors[monitor_manager::active_monitor_idx].workspace_idx);
  }
}
