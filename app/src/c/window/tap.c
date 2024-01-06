#include <pebble.h>
#include "tap.h"

static State* state;

static Window* window;

static TextLayer* text_layer;

static int detected = 0;
static int sum = 0;
static int count = 0;
static time_t last_s = 0;
static uint16_t last_ms = 0;

static void update_text() {
  static char buffer[4];
  snprintf(buffer, 4, "%i", detected);
  text_layer_set_text(text_layer, buffer);
}

static void click_handler(ClickRecognizerRef recognizer, void *context) {
  time_t current_s = 0;
  uint16_t current_ms = 0;
  time_ms(&current_s, &current_ms);
  if (!last_s) {
    last_s = current_s;
    last_ms = current_ms;
    return;
  }
  sum += 60000 / ((current_s - last_s) * 1000 + (current_ms - last_ms));
  count++;
  last_s = current_s;
  last_ms = current_ms;
  detected = sum / count;
  update_text();
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, click_handler);
}

static void window_load(Window* window) {
  detected = 0;
  sum = 0;
  count = 0;
  last_s = 0;
  last_ms = 0;

  Layer *window_layer = window_get_root_layer(window);
  text_layer = text_layer_create(layer_get_bounds(window_layer));
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_layer));
  update_text();
}

static void window_unload(Window* window) {
  if (detected) {
    state->temp = detected;
  }
  text_layer_destroy(text_layer);
}

Window* tap_init(State* stateRef) {
  state = stateRef;
  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  return window;
}

void tap_deinit() {
  window_destroy(window);
}
