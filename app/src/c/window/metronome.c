#include <pebble.h>
#include "metronome.h"

static State* state;

static Window* window;

static AppTimer* timer;

static ActionBarLayer* action_bar;

static GBitmap* action_bar_icon_down;
static GBitmap* action_bar_icon_up;
static GBitmap* music_icon_pause;
static GBitmap* music_icon_play;

static TextLayer* text_layer;

static bool running = false;
static int counter = 0;

static void update_text() {
  static char buffer[4];
  snprintf(buffer, 4, "%i", state->temp);
  text_layer_set_text(text_layer, buffer);
}

static void metronome_loop() {
  timer = app_timer_register(60000 / state->temp, metronome_loop, NULL);
  counter++;
  if (counter % 2 == 0) {
    window_set_background_color(window, GColorWhite);
  } else {
    window_set_background_color(window, GColorLightGray);
  }
  if (quiet_time_is_active()) {
    return;
  }
  vibes_cancel();
  if (state->beat && counter % state->beat == 0) {
    vibes_double_pulse();
  } else {
    vibes_short_pulse();
  }
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (!running) {
    running = true;
    metronome_loop();
    action_bar_layer_set_icon_animated(action_bar, BUTTON_ID_SELECT, music_icon_pause, true);
  } else {
    running = false;
    app_timer_cancel(timer);
    action_bar_layer_set_icon_animated(action_bar, BUTTON_ID_SELECT, music_icon_play, true);
  }
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (state->temp < 999) {
    state->temp++;
    update_text();
  }
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (state->temp > 1) {
    state->temp--;
    update_text();
  }
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_repeating_click_subscribe(BUTTON_ID_UP, 50, up_click_handler);
  window_single_repeating_click_subscribe(BUTTON_ID_DOWN, 50, down_click_handler);
}

static void window_load(Window* window) {
  action_bar_icon_down = gbitmap_create_with_resource(RESOURCE_ID_ACTION_BAR_ICON_DOWN);
  action_bar_icon_up = gbitmap_create_with_resource(RESOURCE_ID_ACTION_BAR_ICON_UP);
  music_icon_pause = gbitmap_create_with_resource(RESOURCE_ID_MUSIC_ICON_PAUSE);
  music_icon_play = gbitmap_create_with_resource(RESOURCE_ID_MUSIC_ICON_PLAY);

  Layer *window_layer = window_get_root_layer(window);
  text_layer = text_layer_create(layer_get_bounds(window_layer));
  text_layer_set_background_color(text_layer, GColorClear);
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_layer));
  update_text();

  action_bar = action_bar_layer_create();
  action_bar_layer_add_to_window(action_bar, window);
  action_bar_layer_set_click_config_provider(action_bar, click_config_provider);
  action_bar_layer_set_icon_animated(action_bar, BUTTON_ID_SELECT, music_icon_play, true);
  action_bar_layer_set_icon_animated(action_bar, BUTTON_ID_UP, action_bar_icon_up, true);
  action_bar_layer_set_icon_animated(action_bar, BUTTON_ID_DOWN, action_bar_icon_down, true);

  running = false;
}

static void window_unload(Window* window) {
  app_timer_cancel(timer);

  action_bar_layer_remove_from_window(action_bar);
  action_bar_layer_destroy(action_bar);

  text_layer_destroy(text_layer);

  gbitmap_destroy(action_bar_icon_down);
  gbitmap_destroy(action_bar_icon_up);
  gbitmap_destroy(music_icon_pause);
  gbitmap_destroy(music_icon_play);
}

Window* metronome_init(State* stateRef) {
  state = stateRef;
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  return window;
}

void metronome_deinit() {
  window_destroy(window);
}
