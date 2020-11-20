#include "config.hpp"
#include "globals.hpp"

#define BINDMOD(mods, code, cmd) key_bindings.push_back(key_binding(SUPER | mods, code, cmd))
#define BIND(code, cmd) key_bindings.push_back(key_binding(SUPER, code, cmd))
#define KEY(name) XKeysymToKeycode(globals::display, name)

namespace config {
  std::vector<key_binding> key_bindings;

  void init() {
    // workspaces
    BIND(KEY(XK_0), "wm_workspace0");
    BIND(KEY(XK_1), "wm_workspace1");
    BIND(KEY(XK_2), "wm_workspace2");
    BIND(KEY(XK_3), "wm_workspace3");
    BIND(KEY(XK_4), "wm_workspace4");
    BIND(KEY(XK_5), "wm_workspace5");
    BIND(KEY(XK_6), "wm_workspace6");
    BIND(KEY(XK_7), "wm_workspace7");
    BIND(KEY(XK_8), "wm_workspace8");
    BIND(KEY(XK_9), "wm_workspace9");
    BIND(KEY(XK_Tab), "wm_tabworkspace");

    // window bindings
    BIND(KEY(XK_H), "wm_cyclewin_l");
    BIND(KEY(XK_L), "wm_cyclewin_r");
    BINDMOD(SHIFT, KEY(XK_H), "wm_cyclestack_l");
    BINDMOD(SHIFT, KEY(XK_L), "wm_cyclestack_r");
    BIND(KEY(XK_Return), "wm_makeprimary");
    BIND(KEY(XK_Q), "wm_kill");

    // command bindings
    BIND(KEY(XK_space), "st");
    BINDMOD(SHIFT, KEY(XK_space), "dmenu_run");
  }
}
