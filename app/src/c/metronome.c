#include <pebble.h>
#include "state/state.h"
#include "window/beat.h"
#include "window/metronome.h"
#include "window/tap.h"

static Window* window;

static Window* beat_window;
static Window* metronome_window;
static Window* tap_window;

static State* state;

static SimpleMenuLayer* menu_layer;

static void start_beat(int index, void *context) {
  window_stack_push(beat_window, true);
}

static void start_metronome(int index, void *context) {
  window_stack_push(metronome_window, true);
}

static void start_tap(int index, void *context) {
  window_stack_push(tap_window, true);
}

static void window_load(Window* window) {
  static const SimpleMenuItem items[3] = {
    {
      .title = "Metronome",
      .subtitle = "Adjust and run",
      .callback = start_metronome,
    },
    {
      .title = "Beat",
      .subtitle = "Change time signature",
      .callback = start_beat,
    },
    {
      .title = "Tap",
      .subtitle = "BPM detector",
      .callback = start_tap,
    },
  };
  static const SimpleMenuSection sections[1] = {
    {
      .title = "Metronome Menu",
      .items = items,
      .num_items = 3,
    },
  };
  Layer* window_layer = window_get_root_layer(window);
  menu_layer = simple_menu_layer_create(layer_get_bounds(window_layer), window, sections, 1, NULL);
  layer_add_child(window_layer, simple_menu_layer_get_layer(menu_layer));
}

static void window_unload(Window* window) {
  simple_menu_layer_destroy(menu_layer);
}

static void init(void) {
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });

  state = state_init();

  beat_window = beat_init(state);
  metronome_window = metronome_init(state);
  tap_window = tap_init(state);

  window_stack_push(window, true);
}

static void deinit(void) {
  beat_deinit();
  metronome_deinit();
  tap_deinit();

  state_write();

  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
