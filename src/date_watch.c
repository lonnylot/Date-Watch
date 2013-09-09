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

typedef struct animation_data {
  PropertyAnimation animate_in;
  char updated_text;
  TextLayer *textlayer;
  GRect from_frame;
  GRect to_frame;
} animation_data;

struct animation_data hour_animation_data;
struct animation_data minute_animation_data;
struct animation_data minute_2_animation_data;

typedef enum {
  ANIMATION_HOUR = 0,
  ANIMATION_MINUTE = 1,
  ANIMATION_MINUTE_2 = 2
} animation_types;

void convert_hours_to_name(unsigned int *number, char *name) {
  switch(*number) {
    case 0: {
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

void convert_minute_to_name(int *number, char *name) {
  if (*number < 20) {
    switch(*number) {
      case 0: {
        strcpy(name, "o'clock");
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
      case 12: {
        strcpy(name, "twelve");
        break;
      }
      case 13: {
        strcpy(name, "thirteen");
        break;
      }
      case 14: {
        strcpy(name, "four");
        break;
      }
      case 15: {
        strcpy(name, "fifteen");
        break;
      }
      case 16: {
        strcpy(name, "sixteen");
        break;
      }
      case 17: {
        strcpy(name, "seven");
        break;
      }
      case 18: {
        strcpy(name, "eight");
        break;
      }
      case 19: {
        strcpy(name, "nine");
        break;
      }
      default: {
        strcpy(name, "");
        break;
      }
    }
  } else if(*number < 30) {
    strcpy(name, "twenty");
  } else if(*number < 40) {
    strcpy(name, "thirty");
  } else if(*number < 50) {
    strcpy(name, "forty");
  } else {
    strcpy(name, "fifty");
  }
}

void convert_minute_2_to_name(int number, char *name) {
  switch(number) {
    case 14:
    case 17:
    case 18:
    case 19: {
      strcpy(name, "teen");
      break;
    }
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8: 
    case 9:
    default: {
      strcpy(name, "");
      break;
    }
  }
}

void current_animation_out_stopped(Animation *animation, bool finished, void *data){
  struct animation_data *animation_info;
  animation_types animation_type;
  animation_type = (int)data;

  switch(animation_type) {
    case ANIMATION_HOUR: {
      animation_info = &hour_animation_data;
      // Update our layer text
      strcpy(current_hour_string, &(*animation_info).updated_text);
      break;
    }
    case ANIMATION_MINUTE: {
      animation_info = &minute_animation_data;
      // Update our layer text
      strcpy(current_minute_string, &(*animation_info).updated_text);
      break;
    }
    case ANIMATION_MINUTE_2:
    default: {
      animation_info = &minute_2_animation_data;
      // Update our layer text
      strcpy(current_minute_2_string, &(*animation_info).updated_text);
      break;
    }
  }
  
  if(animation_info->animate_in.subject == NULL){
    // Initialize our animation layer
    property_animation_init_layer_frame(
      &animation_info->animate_in, &animation_info->textlayer->layer, 
      &animation_info->from_frame, &animation_info->to_frame
    );
    animation_set_duration(&animation_info->animate_in.animation, 500);
    animation_set_curve(&animation_info->animate_in.animation, AnimationCurveEaseIn);
  }

  if(animation_is_scheduled(&animation_info->animate_in.animation) == false) {
    animation_schedule(&animation_info->animate_in.animation);
  }
}

void schedule_hours(int *pebble_hour) {
  static PropertyAnimation animation_out;
  GRect to_frame_out = GRect(-144, 30, 144, 43);
  unsigned int hour;
  static animation_types animation_type = ANIMATION_HOUR;

  // Get our new updated text
  hour = *pebble_hour % 12;
  convert_hours_to_name(&hour, &hour_animation_data.updated_text);

  if ((&animation_out)->subject == NULL) {
    // Initialize our animation layer
    property_animation_init_layer_frame(
      &animation_out, &current_hour.layer, NULL, &to_frame_out
    );
    animation_set_duration(&animation_out.animation, 500);
    animation_set_curve(&animation_out.animation, AnimationCurveEaseOut);
    animation_set_handlers(&animation_out.animation, (AnimationHandlers) {
      .stopped = (AnimationStoppedHandler) current_animation_out_stopped
    }, (void *) animation_type);
  }

  if(animation_is_scheduled(&animation_out.animation) == false) {
    animation_schedule(&animation_out.animation);
  }
}

void schedule_minute(int *pebble_minute) {
  static PropertyAnimation animation_out;
  GRect to_frame_out = GRect(-144, 65, 144, 50);
  static animation_types animation_type = ANIMATION_MINUTE;

  // Get our new updated text
  convert_minute_to_name(pebble_minute, &minute_animation_data.updated_text);

  if ((&animation_out)->subject == NULL) {
    // Initialize our animation layer
    property_animation_init_layer_frame(
      &animation_out, &current_minute.layer, NULL, &to_frame_out
    );
    animation_set_duration(&animation_out.animation, 500);
    animation_set_curve(&animation_out.animation, AnimationCurveEaseOut);
    animation_set_handlers(&animation_out.animation, (AnimationHandlers) {
      .stopped = (AnimationStoppedHandler) current_animation_out_stopped
    }, (void *) animation_type);
  }

  if(animation_is_scheduled(&animation_out.animation) == false) {
    animation_schedule(&animation_out.animation);
  }
}

void schedule_minute_2(int *pebble_minute) {
  static PropertyAnimation animation_out;
  GRect to_frame_out = GRect(-144, 103, 144, 50);
  int min;
  static animation_types animation_type = ANIMATION_MINUTE_2;

  // Get our new updated text
  if(*pebble_minute < 20){
    min = *pebble_minute % 10;
  } else {
    min = *pebble_minute;
  }
  convert_minute_2_to_name(min, &minute_2_animation_data.updated_text);

  if ((&animation_out)->subject == NULL) {
    // Initialize our animation layer
    property_animation_init_layer_frame(
      &animation_out, &current_minute_2.layer, NULL, &to_frame_out
    );
    animation_set_duration(&animation_out.animation, 500);
    animation_set_curve(&animation_out.animation, AnimationCurveEaseOut);
    animation_set_handlers(&animation_out.animation, (AnimationHandlers) {
      .stopped = (AnimationStoppedHandler) current_animation_out_stopped
    }, (void *) animation_type);
  }
  
  if(animation_is_scheduled(&animation_out.animation) == false) {
    animation_schedule(&animation_out.animation);
  }
}

void update_time(PblTm *pebble_time) {
  if(pebble_time->tm_min == 0) {
    // Update hour
    schedule_hours(&(pebble_time->tm_hour));
    // Update minute to "o'clock"
    //schedule_minute(&(pebble_time->tm_min));
    // Update minute_2 to ""
    //schedule_minute_2(&(pebble_time->tm_min));
  } else if(pebble_time->tm_min < 13) {
    // Update minute to ones
    //schedule_minute(&(pebble_time->tm_min));
  } else if(pebble_time->tm_min < 20) {
    // Update minute and minute_2 to teens
    //schedule_minute(&(pebble_time->tm_min));
    //schedule_minute_2(&(pebble_time->tm_min));
  } else if(pebble_time->tm_min % 10 == 0) {
    // Update minute to tens
    //schedule_minute(&(pebble_time->tm_min));
    // Update minute_2 to ""
    //schedule_minute_2(&(pebble_time->tm_min));
  } else {
    // Update minute to tens
    //schedule_minute(&(pebble_time->tm_min));
    // Update minute_2 to ones
    //schedule_minute_2(&(pebble_time->tm_min));
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

  // Set our static animation_data structs from/to fields
  hour_animation_data.from_frame = GRect(144, 30, 144, 43);
  hour_animation_data.to_frame = GRect(0, 30, 144, 43);
  minute_animation_data.from_frame = GRect(144, 65, 144, 50);
  minute_animation_data.to_frame = GRect(0, 65, 144, 50);
  minute_2_animation_data.from_frame = GRect(144, 103, 144, 50);
  minute_2_animation_data.to_frame = GRect(0, 103, 144, 50);

  // Set our static animation_data structs textlayer fields
  hour_animation_data.textlayer = &current_hour;
  minute_animation_data.textlayer = &current_minute;
  minute_2_animation_data.textlayer = &current_minute_2;

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
  //schedule_hours(&(pebble_time.tm_hour));
  //schedule_minute(&(pebble_time.tm_min));
  //schedule_minute_2(&(pebble_time.tm_min));
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
  APP_LOG( APP_LOG_LEVEL_DEBUG, "works!" );
}

void handle_deinit(AppContextRef ctx) {
  (void)ctx;

  animation_unschedule_all();
}


void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,
    .tick_info = {
      .tick_handler = &handle_tick,
      .tick_units = MINUTE_UNIT
    },
    .deinit_handler = &handle_deinit
  };
  app_event_loop(params, &handlers);
}
