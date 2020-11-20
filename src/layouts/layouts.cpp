#include "layouts.hpp"

// incude different layout headers
#include "master_stack.hpp"
#include "monacle.hpp"

namespace layout_manager {
  std::vector<layout*> layouts;
  int current_layout = 0;

  void init() {
    // populate vector with all layouts, default first
    layouts.push_back(new layout_master_stack()); 
    layouts.push_back(new layout_monacle());
  }

  void destroy() {
    for (auto& layout : layouts)
      delete layout;
  }
}
