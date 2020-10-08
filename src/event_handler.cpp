#include "event_handler.h"
#include "config.h"
#include "globals.h"

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
  }

  void dispatch(XEvent& event) {
    
  }
}
