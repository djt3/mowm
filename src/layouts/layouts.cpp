#include "layouts.hpp"

// incude different layout headers
#include "master_stack.hpp"

namespace layout_manager {
  std::vector<layout*> layouts;
  int current_layout = 0;

  void init() {
   layouts.push_back(new layout_master_stack()); 
  }

  void destroy() {
    for (auto& layout : layouts)
      delete layout;
  }
}
