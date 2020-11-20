#include "layouts.hpp"

namespace layout_manager {
  class layout_master_stack : public layout {
  public:
    void update_workspace_layout(int workspace_idx);
  };
}
