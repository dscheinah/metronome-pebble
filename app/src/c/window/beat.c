#include <pebble.h>
#include "beat.h"

static State* state;

static Window* window;

static ActionBarLayer* action_bar;

static GBitmap* action_bar_icon_down;
static GBitmap* action_bar_icon_up;

static TextLayer* text_layer;

static void update_text() {
  static char buffer[3];
  snprintf(buffer, 3, "%i", state->beat);
  text_layer_set_text(text_layer, buffer);
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (state->beat < 99) {
    state->beat++;
    update_text();
  }
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (state->beat > 0) {
    state->beat--;
    update_text();
  }
}

static void click_config_provider(void *context) {
  window_single_repeating_click_subscribe(BUTTON_ID_UP, 50, up_click_handler);
  window_single_repeating_click_subscribe(BUTTON_ID_DOWN, 50, down_click_handler);
}

static void window_load(Window* window) {
  action_bar_icon_down = gbitmap_create_with_resource(RESOURCE_ID_ACTION_BAR_ICON_DOWN);
  action_bar_icon_up = gbitmap_create_with_resource(RESOURCE_ID_ACTION_BAR_ICON_UP);

  Layer *window_layer = window_get_root_layer(window);
  text_layer = text_layer_create(layer_get_bounds(window_layer));
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_layer));
  update_text();

  action_bar = action_bar_layer_create();
  action_bar_layer_add_to_window(action_bar, window);
  action_bar_layer_set_click_config_provider(action_bar, click_config_provider);
  action_bar_layer_set_icon_animated(action_bar, BUTTON_ID_UP, action_bar_icon_up, true);
  action_bar_layer_set_icon_animated(action_bar, BUTTON_ID_DOWN, action_bar_icon_down, true);
}

static void window_unload(Window* window) {
  action_bar_layer_remove_from_window(action_bar);
  action_bar_layer_destroy(action_bar);

  text_layer_destroy(text_layer);

  gbitmap_destroy(action_bar_icon_down);
  gbitmap_destroy(action_bar_icon_up);
}

Window* beat_init(State* stateRef) {
  state = stateRef;
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  return window;
}

void beat_deinit() {
  window_destroy(window);
}
