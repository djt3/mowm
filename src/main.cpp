#include <X11/Xlib.h>
#include <cstring>

#include "globals.h"
#include "config.h"
#include "event_handler.h"
#include "monitor_manager.h"

int main() {
  if (!(globals::display = XOpenDisplay(0)) || !monitor_manager::init())
    exit(1);

  int screen = DefaultScreen(globals::display);
  auto root_window = RootWindow(globals::display, screen);

  XSelectInput(globals::display, root_window, SubstructureRedirectMask | KeyPressMask | SubstructureNotifyMask);
  XDefineCursor(globals::display, root_window, XCreateFontCursor(globals::display, 60));

  // initialize keybinds
  config::init();

  // register keybinds
  for (const auto& keybind : config::key_bindings) {
    XGrabKey(globals::display, keybind.keycode, keybind.mask, root_window, false, GrabModeAsync, GrabModeAsync);

    // we also want to bind workspace + shift
    if (std::strstr(keybind.cmd, "wm_ws"))
      XGrabKey(globals::display, keybind.keycode, keybind.mask | SHIFT, root_window, false, GrabModeAsync, GrabModeAsync);
  }

  while (true) {
    XEvent event;
    XNextEvent(globals::display, &event);

    event_handler::dispatch(event);
  }
}
