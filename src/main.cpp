#include <X11/Xlib.h>
#include <cstring>
#include <iostream>

#include "globals.hpp"
#include "config.hpp"
#include "event_handler.hpp"
#include "monitor_manager.hpp"
#include "layouts/layouts.hpp"

int error_handler(Display* display, XErrorEvent* error) {
  std::cout << "caught error type " << error->type << std::endl;
  return 1;
}

int main() {
  if (!(globals::display = XOpenDisplay(0)) || !monitor_manager::init())
    exit(1);

  int screen = DefaultScreen(globals::display);
  auto root_window = RootWindow(globals::display, screen);

  XSelectInput(globals::display, root_window, SubstructureRedirectMask | KeyPressMask | SubstructureNotifyMask);
  XDefineCursor(globals::display, root_window, XCreateFontCursor(globals::display, 60));

  // ignore X errors
  XSetErrorHandler(&error_handler);

  // initialize keybinds
  config::init();
  // populate layouts
  layout_manager::init();

  // register keybinds
  for (const auto& keybind : config::key_bindings) {
    XGrabKey(globals::display, keybind.keycode, keybind.mask, root_window, false, GrabModeAsync, GrabModeAsync);

    // some keybinds also need to grab mod + shift
    if (keybind.has_shift_binding())
      XGrabKey(globals::display, keybind.keycode, keybind.mask | SHIFT, root_window, false, GrabModeAsync, GrabModeAsync);
  }

  while (true) {
    XEvent event;
    XNextEvent(globals::display, &event);

    event_handler::dispatch(event);
  }

  layout_manager::destroy();
}
