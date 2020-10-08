#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <X11/keysym.h>
#include <X11/Xlib.h>
#include <vector>
#include <cstring>
#include <cstdint>

enum class wm_commands : uint8_t {
  none = 0,
  workspace,
  tabmonitor
};

struct key_binding {
key_binding(unsigned mask, unsigned keycode, const char* cmd) :
  mask(mask), keycode(keycode), cmd(cmd)
  {
    if (std::strstr(this->cmd, "wm_workspace"))
      this->wm_command = wm_commands::workspace;
    if (std::strstr(this->cmd, "wm_tabmon"))
      this->wm_command = wm_commands::tabmonitor;
  }

  bool triggered_by(const XKeyEvent& key_event) const {
    return key_event.keycode == this->keycode &&
      (key_event.state == this->mask || has_shift_binding() && key_event.state == this->mask | ShiftMask);
  }

  bool has_shift_binding() const {
    return this->wm_command == wm_commands::workspace ||
      this->wm_command == wm_commands::tabmonitor;
  }

  bool is_wm_command() const {
    return this->wm_command != wm_commands::none;
  }

  unsigned int mask, keycode;
  const char* cmd;
  wm_commands wm_command = wm_commands::none;
};

#define SUPER Mod4Mask // start/windows button
#define ALT Mod1Mask
#define CONTROL ControlMask
#define SHIFT ShiftMask

namespace config {
  extern std::vector<key_binding> key_bindings;

  extern void init();
}

#endif
