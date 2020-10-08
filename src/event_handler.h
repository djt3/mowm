#ifndef _EVENT_HANDLER_H_
#define _EVENT_HANDLER_H_

#include <X11/Xlib.h>

namespace event_handler {
  extern void dispatch(XEvent& event);
}

#endif
