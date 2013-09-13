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

void convert_hours_to_name(unsigned int *number) {
  switch(*number) {
    case 0: {
      strcpy(current_hour_string, "twelve");
      break;
    }
    case 1: {
      strcpy(current_hour_string, "one");
      break;
    }
    case 2: {
      strcpy(current_hour_string, "two");
      break;
    }
    case 3: {
      strcpy(current_hour_string, "three");
      break;
    }
    case 4: {
      strcpy(current_hour_string, "four");
      break;
    }
    case 5: {
      strcpy(current_hour_string, "five");
      break;
    }
    case 6: {
      strcpy(current_hour_string, "six");
      break;
    }
    case 7: {
      strcpy(current_hour_string, "seven");
      break;
    }
    case 8: {
      strcpy(current_hour_string, "eight");
      break;
    }
    case 9: {
      strcpy(current_hour_string, "nine");
      break;
    }
    case 10: {
      strcpy(current_hour_string, "ten");
      break;
    }
    case 11: {
      strcpy(current_hour_string, "eleven");
      break;
    }
    default: {
      strcpy(current_hour_string, "");
      break;
    }
  }
}

void convert_minute_to_name(int *number) {
  if (*number < 20) {
    switch(*number) {
      case 0: {
        strcpy(current_minute_string, "o'clock");
        break;
      }
      case 1: {
        strcpy(current_minute_string, "one");
        break;
      }
      case 2: {
        strcpy(current_minute_string, "two");
        break;
      }
      case 3: {
        strcpy(current_minute_string, "three");
        break;
      }
      case 4: {
        strcpy(current_minute_string, "four");
        break;
      }
      case 5: {
        strcpy(current_minute_string, "five");
        break;
      }
      case 6: {
        strcpy(current_minute_string, "six");
        break;
      }
      case 7: {
        strcpy(current_minute_string, "seven");
        break;
      }
      case 8: {
        strcpy(current_minute_string, "eight");
        break;
      }
      case 9: {
        strcpy(current_minute_string, "nine");
        break;
      }
      case 10: {
        strcpy(current_minute_string, "ten");
        break;
      }
      case 11: {
        strcpy(current_minute_string, "eleven");
        break;
      }
      case 12: {
        strcpy(current_minute_string, "twelve");
        break;
      }
      case 13: {
        strcpy(current_minute_string, "thirteen");
        break;
      }
      case 14: {
        strcpy(current_minute_string, "four");
        break;
      }
      case 15: {
        strcpy(current_minute_string, "fifteen");
        break;
      }
      case 16: {
        strcpy(current_minute_string, "sixteen");
        break;
      }
      case 17: {
        strcpy(current_minute_string, "seven");
        break;
      }
      case 18: {
        strcpy(current_minute_string, "eight");
        break;
      }
      case 19: {
        strcpy(current_minute_string, "nine");
        break;
      }
      default: {
        strcpy(current_minute_string, "");
        break;
      }
    }
  } else if(*number < 30) {
    strcpy(current_minute_string, "twenty");
  } else if(*number < 40) {
    strcpy(current_minute_string, "thirty");
  } else if(*number < 50) {
    strcpy(current_minute_string, "forty");
  } else {
    strcpy(current_minute_string, "fifty");
  }
}

void convert_minute_2_to_name(int number) {
  switch(number) {
    case 14:
    case 17:
    case 18:
    case 19: {
      strcpy(current_minute_2_string, "teen");
      break;
    }
    case 1: {
      strcpy(current_minute_2_string, "one");
      break;
    }
    case 2: {
      strcpy(current_minute_2_string, "two");
      break;
    }
    case 3: {
      strcpy(current_minute_2_string, "three");
      break;
    }
    case 4: {
      strcpy(current_minute_2_string, "four");
      break;
    }
    case 5: {
      strcpy(current_minute_2_string, "five");
      break;
    }
    case 6: {
      strcpy(current_minute_2_string, "six");
      break;
    }
    case 7: {
      strcpy(current_minute_2_string, "seven");
      break;
    }
    case 8:  {
      strcpy(current_minute_2_string, "eight");
      break;
    }
    case 9: {
      strcpy(current_minute_2_string, "nine");
      break;
    }
    case 0:
    default: {
      strcpy(current_minute_2_string, "");
      break;
    }
  }
}

void schedule_hours(int *pebble_hour) {
  unsigned int hour;

  // Get our new updated text
  hour = *pebble_hour % 12;
  convert_hours_to_name(&hour);

  text_layer_set_text(&current_hour, current_hour_string);
}

void schedule_minute(int *pebble_minute) {
  // Get our new updated text
  convert_minute_to_name(pebble_minute);

  text_layer_set_text(&current_minute, current_minute_string);
}

void schedule_minute_2(int *pebble_minute) {
  int min;

  // Get our new updated text
  if(*pebble_minute > 20 && *pebble_minute % 10 != 0) {
    min = *pebble_minute % 10;
    convert_minute_2_to_name(min);
  } else if(*pebble_minute == 14) {
    convert_minute_2_to_name(*pebble_minute);
  } else if(*pebble_minute >= 17 && *pebble_minute <= 19){
    convert_minute_2_to_name(*pebble_minute);
  } else {
    convert_minute_2_to_name(0);
  }

  text_layer_set_text(&current_minute_2, current_minute_2_string);
}

void update_time(PblTm *pebble_time) {
  if(pebble_time->tm_min == 0) {
    // Update hour
    schedule_hours(&(pebble_time->tm_hour));
    // Update minute to "o'clock"
    schedule_minute(&(pebble_time->tm_min));
    // Update minute_2 to ""
    schedule_minute_2(&(pebble_time->tm_min));
  } else if(pebble_time->tm_min < 13) {
    // Update minute to ones
    schedule_minute(&(pebble_time->tm_min));
  } else if(pebble_time->tm_min < 20) {
    // Update minute and minute_2 to teens
    schedule_minute(&(pebble_time->tm_min));
    schedule_minute_2(&(pebble_time->tm_min));
  } else if(pebble_time->tm_min % 10 == 0) {
    // Update minute to tens
    schedule_minute(&(pebble_time->tm_min));
    // Update minute_2 to ""
    schedule_minute_2(&(pebble_time->tm_min));
  } else {
    // Update minute to tens
    schedule_minute(&(pebble_time->tm_min));
    // Update minute_2 to ones
    schedule_minute_2(&(pebble_time->tm_min));
  }
}

void update_date(PblTm *pebble_time) {
  string_format_time(current_date_string, DATE_TEXT_CHARS, "%a, %d", pebble_time);
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
  minute_font = fonts_get_system_font(FONT_KEY_BITHAM_42_LIGHT);
  text_layer_set_font(&current_minute, minute_font);
  text_layer_set_font(&current_minute_2, minute_font);
  text_layer_set_font(&current_hour, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_font(&current_date, fonts_get_system_font(FONT_KEY_GOTHIC_24));

  // Do the initial rendering of our layer content
  PblTm pebble_time;
  get_time(&pebble_time);
  schedule_hours(&(pebble_time.tm_hour));
  schedule_minute(&(pebble_time.tm_min));
  schedule_minute_2(&(pebble_time.tm_min));
  update_date(&pebble_time);
  
  // Set the text for our text layers
  text_layer_set_text(&current_minute_2, current_minute_2_string);
  text_layer_set_text(&current_minute, current_minute_string);
  text_layer_set_text(&current_hour, current_hour_string);
  text_layer_set_text(&current_date, current_date_string);

  // Add layers as children of the window layer
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
