#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
#include <math.h>


#define MY_UUID { 0x95, 0x85, 0xE1, 0x41, 0x5E, 0xF7, 0x45, 0x34, 0x86, 0x2F, 0xB1, 0x7B, 0xA8, 0xFA, 0x16, 0x59 }
PBL_APP_INFO(MY_UUID,
             "Date Text", "L L Ideas, LLC",
             1, 0, /* App version */
             INVALID_RESOURCE,
             APP_INFO_WATCH_FACE);

Window window;

TextLayer current_date;
TextLayer current_hour;
TextLayer current_minute;
TextLayer current_minute_2;

#define HOUR_TEXT_CHARS 7
char current_hour_string[HOUR_TEXT_CHARS];

#define MINUTE_TEXT_CHARS 7
char current_minute_string[MINUTE_TEXT_CHARS];

#define MINUTE_2_TEXT_CHARS 6
char current_minute_2_string[MINUTE_2_TEXT_CHARS];

#define DATE_TEXT_CHARS 8
char current_date_string[DATE_TEXT_CHARS];

void convert_ones_to_name(unsigned int *number, char *name) {
  switch(*number) {
    case 0: 
    case 12: {
      strcpy(name, "twelve");
      break;
    }
    case 1: {
      strcpy(name, "one");
      break;
    }
    case 2: {
      strcpy(name, "two");
      break;
    }
    case 3: {
      strcpy(name, "three");
      break;
    }
    case 4: {
      strcpy(name, "four");
      break;
    }
    case 5: {
      strcpy(name, "five");
      break;
    }
    case 6: {
      strcpy(name, "six");
      break;
    }
    case 7: {
      strcpy(name, "seven");
      break;
    }
    case 8: {
      strcpy(name, "eight");
      break;
    }
    case 9: {
      strcpy(name, "nine");
      break;
    }
    case 10: {
      strcpy(name, "ten");
      break;
    }
    case 11: {
      strcpy(name, "eleven");
      break;
    }
    default: {
      strcpy(name, "");
      break;
    }
  }
}

void convert_tens_to_name(unsigned int *number, char *name) {
  switch(*number) {
    case 2: {
      strcpy(name, "twenty");
      break;
    }
    case 3: {
      strcpy(name, "thirty");
      break;
    }
    case 4: {
      strcpy(name, "fourty");
      break;
    }
    case 5: {
      strcpy(name, "fifty");
      break;
    }
    default: {
      strcpy(name, "");
      break;
    }
  }
}

void convert_to_teen_name(int *number, char *name, char *name_2) {
  switch(*number) {
    case 13: {
      strcpy(name, "thirteen");
      strcpy(name_2, "");
      break;
    }
    case 14: {
      strcpy(name, "four");
      strcpy(name_2, "teen");
      break;
    }
    case 15: {
      strcpy(name, "fifteen");
      strcpy(name_2, "");
      break;
    }
    case 16: {
      strcpy(name, "sixteen");
      strcpy(name_2, "");
      break;
    }
    case 17: {
      strcpy(name, "seven");
      strcpy(name_2, "teen");
      break;
    }
    case 18: {
      strcpy(name, "eight");
      strcpy(name_2, "teen");
      break;
    }
    case 19: {
      strcpy(name, "nine");
      strcpy(name_2, "teen");
      break;
    }
    default: {
      strcpy(name, "");
      strcpy(name_2, "");
      break;
    }
  }
}

void update_time(PblTm *pebble_time) {
  unsigned int hour, minute, minute_2;
  static PropertyAnimation minute_2_animation;

  // Get our hour string
  hour = pebble_time->tm_hour % 12;
  convert_ones_to_name(&hour, current_hour_string);
  
  // Set our hour text
  text_layer_set_text(&current_hour, current_hour_string);

  // Get our minute and minute_2 strings
  if(pebble_time->tm_min == 0) {
    strcpy(current_minute_string, "o'clock");
    strcpy(current_minute_2_string, "");
  } else if( pebble_time->tm_min < 13 ) {
    // Handle the sub 'teens'
    minute = pebble_time->tm_min;  
    convert_ones_to_name(&minute, current_minute_string);
  } else if( pebble_time->tm_min < 20 ) {
    // Handle the 'teens'
    convert_to_teen_name(&(pebble_time->tm_min), current_minute_string, current_minute_2_string);
  } else {
    // Handle the 'tens'
    minute = (int)(pebble_time->tm_min * .1);
    convert_tens_to_name(&minute, current_minute_string);
    minute_2 = pebble_time->tm_min % 10;
    convert_ones_to_name(&minute_2, current_minute_2_string);
  }

  // Set our minute text
  text_layer_set_text(&current_minute, current_minute_string);
  text_layer_set_text(&current_minute_2, current_minute_2_string);
}

void update_date(PblTm *pebble_time) {
  string_format_time(current_date_string, DATE_TEXT_CHARS, "%a, %d", pebble_time);
  text_layer_set_text(&current_date, current_date_string);
}

void handle_tick(AppContextRef ctx, PebbleTickEvent *event){
  update_time(event->tick_time);
  if(event->tick_time->tm_hour == 0 && event->tick_time->tm_min == 0){
    update_date(event->tick_time);
  }
}

void handle_init(AppContextRef ctx) {
  (void)ctx;

  window_init(&window, "Date Text");
  window_stack_push(&window, true /* Animated */);
  window_set_background_color(&window, GColorBlack);

  // Init our hour layer
  text_layer_init(&current_hour, GRect(0, 30, 144, 43));
  text_layer_set_background_color(&current_hour, GColorBlack);
  text_layer_set_text_color(&current_hour, GColorWhite);

  // Init our minute layer
  text_layer_init(&current_minute, GRect(0, 65, 144, 50));
  text_layer_set_background_color(&current_minute, GColorBlack);
  text_layer_set_text_color(&current_minute, GColorWhite);

  // Init our minute_2 layer
  text_layer_init(&current_minute_2, GRect(0, 103, 144, 50));
  text_layer_set_background_color(&current_minute_2, GColorBlack);
  text_layer_set_text_color(&current_minute_2, GColorWhite);  

  // Init our date layer
  GRect root_frame = layer_get_frame(window_get_root_layer(&window));
  text_layer_init(&current_date, GRect((root_frame.size.w * .5)-30, 5, 144, 25));
  text_layer_set_background_color(&current_date, GColorBlack);
  text_layer_set_text_color(&current_date, GColorWhite);

  // Set our font
  GFont minute_font;
  minute_font = fonts_get_system_font(FONT_KEY_GOTHAM_42_LIGHT);
  text_layer_set_font(&current_minute, minute_font);
  text_layer_set_font(&current_minute_2, minute_font);
  text_layer_set_font(&current_hour, fonts_get_system_font(FONT_KEY_GOTHAM_42_BOLD));
  text_layer_set_font(&current_date, fonts_get_system_font(FONT_KEY_GOTHIC_24));

  // Do the initial rendering of our layer content
  PblTm pebble_time;
  get_time(&pebble_time);
  update_time(&pebble_time);
  update_date(&pebble_time);

  // Add layers as children
  layer_add_child(&window.layer, &current_minute_2.layer);
  layer_add_child(&window.layer, &current_minute.layer);
  layer_add_child(&window.layer, &current_hour.layer);
  layer_add_child(&window.layer, &current_date.layer);
}


void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,
    .tick_info = {
      .tick_handler = &handle_tick,
      .tick_units = MINUTE_UNIT
    }
  };
  app_event_loop(params, &handlers);
}
