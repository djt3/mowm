#include "event_handler.h"
#include "config.h"
#include "globals.h"
#include "layout_manager.h"

#include <cstring>
#include <sys/types.h>
#include <unistd.h>

namespace event_handler {
  namespace events {
    void key_press(const XKeyEvent& key_event) {
      for (const auto& keybind : config::key_bindings) {
        if (!keybind.triggered_by(key_event))
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
          break;
        }

        if (keybind.wm_command == wm_commands::workspace) {

        }

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
      layout_manager::destroy_window(event.window);
    }
  }

  void dispatch(XEvent& event) {
    if (event.type == KeyPress)
      events::key_press(event.xkey);
    else if (event.type == MapRequest)
      events::map_request(event.xmaprequest);
    else if (event.type == DestroyNotify)
      events::destroy_notify(event.xdestroywindow);
  }
}
