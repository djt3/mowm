#ifndef _MONITOR_MANAGER_H_
#define _MONITOR_MANAGER_H_

#include <X11/extensions/Xinerama.h>
#include <vector>

struct dimensions {
  unsigned int width, height;
};

struct position {
  unsigned int x, y;
};

struct monitor {
  monitor(const XineramaScreenInfo& screen_info) {
    this->dim.width = screen_info.width;
    this->dim.height = screen_info.height;

    this->pos.x = screen_info.x_org;
    this->pos.y = screen_info.y_org;
  }

  dimensions dim;
  position pos;
  int workspace_idx = 0;
};

namespace monitor_manager {
  extern std::vector<monitor> monitors;

  extern bool init();
  extern int get_monitor_count(bool force_update = false);
}

#endif
