#ifndef _LAYOUTS_HPP_
#define _LAYOUTS_HPP_

#include <vector>

class layout {
public:
  virtual void update_workspace_layout(int workspace_idx) = 0;
};

namespace layout_manager {
  extern void init();
  extern void destroy();

  extern std::vector<layout*> layouts;
}

#endif
