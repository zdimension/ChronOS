#include <kernel/desktop.h>
#include <kernel/rect.h>
#include <kernel/video.h>

Window *Desktop::createWindow (uint16_t x, uint16_t y, uint16_t width, uint16_t height) {
  Window *window = new Window (this->context, x, y, width, height);

  this->windows->insert (window);

  return window;
}

void Desktop::paint () {
  size_t i;
  Window *current, *clipping_win;
  Rect *temp_rect;
  List<Window> *clip_windows;

  temp_rect = new Rect (0, 0, this->context->GetHeight () - 1, this->context->GetWidth () - 1);
  this->context->add_clip_rect (temp_rect);

  for (i = 0; i < this->windows->getSize (); i++) {
    current = this->windows->get (i);

    temp_rect = new Rect (current->getY (), current->getX (),
                     current->getY () + current->getHeight () - 1,
                     current->getX () + current->getWidth () - 1);

    this->context->subtract_clip_rect (temp_rect);

    delete temp_rect;
  }
  this->context->fill_rect (0, 0, this->context->GetWidth (), this->context->GetHeight (), 0x3399FF);

  this->context->clear_clip_rects ();

  for (i = 0; i < this->windows->getSize (); i++) {
    current = this->windows->get (i);

    temp_rect = new Rect (current->getY (), current->getX (),
                     current->getY () + current->getHeight () - 1,
                     current->getX () + current->getWidth () - 1);

    this->context->add_clip_rect (temp_rect);

    clip_windows = this->getWindowsAbove (current);

    while (clip_windows->getSize ()) {
      clipping_win = clip_windows->remove (0);

      if (clipping_win == current)
        continue;

      temp_rect = new Rect (clipping_win->getY (), clipping_win->getX (),
                       clipping_win->getY () + clipping_win->getHeight () - 1,
                       clipping_win->getX () + clipping_win->getWidth () - 1);

      this->context->subtract_clip_rect (temp_rect);

      delete temp_rect;
    }

    current->paint ();

    delete clip_windows;
    this->context->clear_clip_rects ();
  }

  this->context->fill_rect (this->mouse_x, this->mouse_y, 10, 10, 0xFFFFFFFF);
  swap_buffers (this->context->getVGA (), 0, 0, this->context->GetWidth (), this->context->GetHeight ());
}

void Desktop::update (int16_t mouse_x, int16_t mouse_y, uint8_t mouse_buttons) {
  int i;
  Window *child;

  this->mouse_x = mouse_x;
  this->mouse_y = mouse_y;

  if (mouse_buttons) {
    if (!this->last_buttons_state)
      for (i = this->windows->getSize () - 1; i >= 0; i--) {
        child = this->windows->get (i);

        if (mouse_x >= child->getX () && mouse_x < (child->getX () + child->getWidth ()) &&
           mouse_y >= child->getY () && mouse_y < (child->getY () + 12)) {
             this->windows->remove (i);
             this->windows->insert (child);

             this->drag_off_x = mouse_x - child->getX ();
             this->drag_off_y = mouse_y - child->getY ();
             this->dragged_window = child;

             break;
           }
      }
  } else {
    this->dragged_window = nullptr;
  }

  if (this->dragged_window) {
    this->dragged_window->setX (mouse_x - this->drag_off_x);
    this->dragged_window->setY (mouse_y - this->drag_off_y);
  }

  this->paint ();

  this->last_buttons_state = mouse_buttons;
}

List<Window> *Desktop::getWindowsAbove (Window *window) {
  size_t i;
  Window *current;
  List<Window> *return_list = new List<Window> ();

  for (i = 0; i < this->windows->getSize (); i++)
    if (window == this->windows->get (i))
      break;

  for (; i < this->windows->getSize (); i++) {
    current = this->windows->get (i);

    if (current->getX () <= (window->getX () + window->getWidth() - 1) &&
       (current->getX () + current->getWidth () - 1) >= window->getX () &&
       current->getY () <= (window->getY () + window->getHeight () - 1) &&
       (window->getY () + window->getHeight() - 1) >= window->getY ())
        return_list->insert (current);
  }

  return return_list;
}
