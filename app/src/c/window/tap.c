#include <pebble.h>
#include "tap.h"

static State* state;

static Window* window;

static TextLayer* text_layer;
static TextLayer* label_layer;

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
  GRect bounds = layer_get_bounds(window_layer);

  text_layer = text_layer_create(GRect(0, bounds.size.h / 2 - 28, bounds.size.w, 84));
  text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_MEDIUM_NUMBERS));
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  update_text();

  label_layer = text_layer_create(GRect(0, 6, bounds.size.w, 36));
  text_layer_set_font(label_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_text_alignment(label_layer, GTextAlignmentCenter);
  text_layer_set_text(label_layer, "Tap tempo with clicks");

  layer_add_child(window_layer, text_layer_get_layer(text_layer));
  layer_add_child(window_layer, text_layer_get_layer(label_layer));
}

static void window_unload(Window* window) {
  if (detected) {
    state->temp = detected;
  }
  text_layer_destroy(text_layer);
  text_layer_destroy(label_layer);
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
