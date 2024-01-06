#include <pebble.h>
#include "state.h"

#define VERSION_KEY 0
#define STATE_KEY   1

static State state = {
  .temp = 60,
  .beat = 0,
};

State* state_init() {
  if (persist_exists(VERSION_KEY)) {
    persist_read_data(STATE_KEY, &state, sizeof(State));
  }
  return &state;
}

void state_write() {
  persist_write_int(VERSION_KEY, 1);
  persist_write_data(STATE_KEY, &state, sizeof(State));
}
