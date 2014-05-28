#include <pebble.h>
//#include <math.h>

static Window *window;
static TextLayer *text_layer;

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "Select");
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "Up");
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "Down");
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  text_layer = text_layer_create((GRect) { .origin = { 0, 72 }, .size = { bounds.size.w, 20 } });
  text_layer_set_text(text_layer, "Press a button");
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_layer));
}

static void window_unload(Window *window) {
  text_layer_destroy(text_layer);
}

static int turn = 0;

float float_sqrt(float num) {
    float e = 0.001;
    int max_steps = 20;
    int steps = 0;
    float a,p,b;
    a = num;
    p = a * a;
    while (p - num >= e && steps < max_steps) {
        b = (a + (num / a)) / 2;
        a = b;
        p = a * a;
        steps++;
    }

    return a;

}
void accel_data_handler(AccelData* data, uint32_t num_samples) {
    // handle data 
    float magnitude = 0;
    float average = 0;
    float sum = 0;
    float varianceSum = 0;
    app_log(APP_LOG_LEVEL_INFO, "activity-tracker.c", 56, "Received data");
    for (uint32_t i = 0; i < num_samples - 1; i+=2) {
        AccelData sample1 = data[i];
        AccelData sample2 = data[i+1];
        uint32_t n = i / 2;
        float x = ((float)sample1.x + (float)sample2.x) / 204.0f;
        float y = ((float)sample1.y + (float)sample2.y) / 204.0f;
        float z = ((float)sample1.z + (float)sample2.z) / 204.0f;
        //app_log(APP_LOG_LEVEL_INFO, "activity-tracker.c", 63, "%i,%i,%i", x,y,z);

        magnitude = float_sqrt(x * x + y * y + z * z);
        //app_log(APP_LOG_LEVEL_INFO, "activity-tracker.c", 69, "m: %i", (int)magnitude);
        float newAverage = (n*average + magnitude) / (n + 1);
        float deltaAverage = newAverage - average;
        float variance = magnitude - newAverage;
        //app_log(APP_LOG_LEVEL_INFO, "activity-tracker.c", 73, "a: %i",(int) newAverage);
        varianceSum += variance*variance - 2*(sum - n*average) + n*deltaAverage*deltaAverage;
        sum += magnitude;
        average = newAverage;       
    }

    //magnitude = magnitude / num_samples;
    char* buffer = (char *) malloc(10 * sizeof(char));
    //ftoa(buffer, magnitude);
    snprintf(buffer, 10, "%i",(int)varianceSum);
    app_log(APP_LOG_LEVEL_INFO, "activity-tracker.c", 51, "varianceSum: %i", (int)varianceSum);
    //text_layer_set_text(text_layer, buffer);
    free(buffer);
    //if (turn % 2 == 0) {
    //    text_layer_set_text(text_layer, "Received");
    //} else {
    //    text_layer_set_text(text_layer, "Received again");
    //}
    turn++;
}

static void init(void) {
  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  accel_service_set_sampling_rate(ACCEL_SAMPLING_10HZ);
  accel_data_service_subscribe(10, &accel_data_handler);
  window_stack_push(window, animated);
}

static void deinit(void) {
    accel_data_service_unsubscribe();
    window_destroy(window);
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}
