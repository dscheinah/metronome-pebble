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
static TextLayer* beat_layer;

static bool running = false;
static int counter = 0;

static void update_text() {
  static char buffer[4];
  snprintf(buffer, 4, "%i", state->temp);
  text_layer_set_text(text_layer, buffer);
  if (state->beat) {
    static char beatBuffer[7];
    snprintf(beatBuffer, 7, "%i/%i", (counter % state->beat) + 1, state->beat);
    text_layer_set_text(beat_layer, beatBuffer);
  }
}

static void metronome_loop() {
  timer = app_timer_register(60000 / state->temp, metronome_loop, NULL);
  counter++;
  if (counter % 2 == 0) {
    window_set_background_color(window, GColorWhite);
  } else {
    window_set_background_color(window, GColorLightGray);
  }
  update_text();
  if (quiet_time_is_active()) {
    return;
  }
  vibes_cancel();
  if (state->beat && counter % state->beat == 0) {
    static const uint32_t beatDurations[1] = {100};
    static const VibePattern beatVibe = {
      .durations = beatDurations,
      .num_segments = 1,
    };
    vibes_enqueue_custom_pattern(beatVibe);
  } else {
    static const uint32_t durations[1] = {50};
    static const VibePattern vibe = {
      .durations = durations,
      .num_segments = 1,
    };
    vibes_enqueue_custom_pattern(vibe);
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
  if (state->temp < 500) {
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
  GRect bounds = layer_get_bounds(window_layer);

  text_layer = text_layer_create(GRect(0, bounds.size.h / 2 - 28, bounds.size.w - ACTION_BAR_WIDTH, 84));
  text_layer_set_background_color(text_layer, GColorClear);
  text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_MEDIUM_NUMBERS));
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);

  beat_layer = text_layer_create(GRect(12, bounds.size.h - 36, bounds.size.w - ACTION_BAR_WIDTH - 12, 36));
  text_layer_set_background_color(beat_layer, GColorClear);
  text_layer_set_font(beat_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));

  update_text();

  layer_add_child(window_layer, text_layer_get_layer(text_layer));
  layer_add_child(window_layer, text_layer_get_layer(beat_layer));

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
  text_layer_destroy(beat_layer);

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
