#include <pebble.h>
#include "beat.h"

static State* state;

static NumberWindow* number_window;

static void select(struct NumberWindow *number_window, void *context) {
  state->beat = number_window_get_value(number_window);
  window_stack_pop(true);
}

Window* beat_init(State* stateRef) {
  state = stateRef;
  number_window = number_window_create("Set beat steps", (NumberWindowCallbacks) {.selected = select}, NULL);
  number_window_set_min(number_window, 0);
  number_window_set_max(number_window, 99);
  beat_update();
  return number_window_get_window(number_window);
}

void beat_update() {
  number_window_set_value(number_window, state->beat);
}

void beat_deinit() {
  number_window_destroy(number_window);
}
