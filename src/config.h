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
    tab_workspace,
    tab_monitor,
    tab_win,
    kill_window,
    make_primary,
};

struct key_binding {
key_binding(unsigned mask, unsigned keycode, const char* cmd) :
  mask(mask), keycode(keycode), cmd(cmd)
  {
    if (this->cmd[0] != 'w' || this->cmd[1] != 'm')
      return;

    if (std::strstr(this->cmd, "wm_workspace"))
      this->wm_command = wm_commands::workspace;
    else if (std::strstr(this->cmd, "wm_tabmon"))
      this->wm_command = wm_commands::tab_monitor;
    else if (std::strstr(this->cmd, "wm_tabwork"))
      this->wm_command = wm_commands::tab_workspace;
    else if (std::strstr(this->cmd, "wm_kill"))
      this->wm_command = wm_commands::kill_window;
    else if (std::strstr(this->cmd, "wm_tabwin"))
      this->wm_command = wm_commands::tab_win;
    else if (std::strstr(this->cmd, "wm_makeprimary"))
      this->wm_command = wm_commands::make_primary;
  }

  bool triggered_by(const XKeyEvent& event) const {
    return event.keycode == this->keycode &&
      (event.state == this->mask || (has_shift_binding() && this->is_shifted(event)));
  }

  bool is_shifted(const XKeyEvent& event) const {
    return event.state == (this->mask | ShiftMask);
  }

  bool has_shift_binding() const {
    return this->wm_command == wm_commands::workspace;
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
