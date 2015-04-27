// Pixel Dogfight for Pebble
// Made by Ben Chapman-Kish from 2015-04-09 to 2015-04-22
#include "pebble.h"
#include "plane.h"
#include "weapons.h"

#define REFRESH_TIMER 50
#define BUTTON_REPEAT_TIMER 150
#define END_GAME_TIMER 1000

#define NUM_FIRE_MODES 2

static Window *main_menu_w, *main_game_w;
static MenuLayer *main_menu_l;
static Layer *game_layer;
static GBitmap *bg_bitmap;
static BitmapLayer *bg_layer;
static bool game_running = false, game_over = false;
static int p1attack = 0, p1firing = 0, p2attack = 0;
static Plane *Player1;
static Bullet *bullets;

static char *fire_modes[] = {"Normal Bullet", "Bullet Burst"};

void game_render(Layer *layer, GContext *ctx) {
	if (game_over) {
		static char end_text_buffer[16];
		if (Player1->crashed) {
			snprintf(end_text_buffer, sizeof(end_text_buffer), "You lose!");
		}
		graphics_context_set_fill_color(ctx, GColorBlack);
		graphics_fill_rect(ctx, (GRect) {.origin = {15, 30}, .size = {144 - 30, 154 - 60}}, 0, GCornerNone);
		graphics_context_set_fill_color(ctx, GColorWhite);
		graphics_fill_rect(ctx, (GRect) {.origin = {17, 32}, .size = {144 - 34, 154 - 64}}, 0, GCornerNone);
		graphics_context_set_text_color(ctx, GColorBlack);
		graphics_draw_text(ctx, end_text_buffer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD), \
			(GRect) {.origin = {10, 58}, .size = {144 - 20, 154 - 20}}, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
	} else {
		bullets_render(bullets, ctx);
		plane_render(Player1, ctx);
	}
}

static void end_game(void *data) {
	game_running = false;
	game_over = true;
	layer_mark_dirty(game_layer);
}

static void update_game_frame(void *data) {
	if (Player1->crashed) {
		app_timer_register(END_GAME_TIMER, end_game, NULL);
	} else {
		plane_move(Player1);
		bullets_move(bullets);
		if (!Player1->crashed) {
			if (Player1->cdwn > 0) {
				--(Player1->cdwn);
			} else if (p1firing > 0) {
				bullets = bullet_create(bullets, Player1->player, Player1->x, Player1->y, Player1->rot);
				if (p1attack == 0 || Player1->burstrepeat >= 2) {
					Player1->cdwn = Player1->firerate;
					Player1->burstrepeat = 0;
					if (p1firing == 1) {
						--p1firing;
					}
				} else if (p1attack == 1) {
					Player1->cdwn = 2;
					++(Player1->burstrepeat);
				}
			}
		}
	}
	layer_mark_dirty(game_layer);
	if (game_running) {
		app_timer_register(REFRESH_TIMER, update_game_frame, NULL);
	}	
}

static void single_shot_handler(ClickRecognizerRef recognizer, void *context) {
	p1firing = 1;
}

static void repeat_shot_handler(ClickRecognizerRef recognizer, void *context) {
	if (p1firing < 2) {
		p1firing = 2;
		vibes_short_pulse();
	} else {
		p1firing = 0;
	}
}

static void rot_up_handler(ClickRecognizerRef recognizer, void *context) {
	Player1->rot += 1;
	if (Player1->rot > 15) {
		Player1->rot = 0;
	}
	update_bitmaps(Player1);
}

static void rot_down_handler(ClickRecognizerRef recognizer, void *context) {
	Player1->rot -= 1;
	if (Player1->rot < 0) {
		Player1->rot = 15;
	}
	update_bitmaps(Player1);
}

static void click_config_provider(void *context) {
	window_single_click_subscribe(BUTTON_ID_SELECT, single_shot_handler);
	window_long_click_subscribe(BUTTON_ID_SELECT, 0, repeat_shot_handler, NULL);
	window_single_repeating_click_subscribe(BUTTON_ID_UP, BUTTON_REPEAT_TIMER, rot_up_handler);
	window_single_repeating_click_subscribe(BUTTON_ID_DOWN, BUTTON_REPEAT_TIMER, rot_down_handler);
}




static uint16_t menu_get_num_sections(MenuLayer *menu_layer, void *data) {
  return 1;
}
static uint16_t menu_get_num_rows(MenuLayer *menu_layer, uint16_t section_index, void *data) {
	return 4;
}
static int16_t menu_get_header_height(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  return MENU_CELL_BASIC_HEADER_HEIGHT;
}
static void menu_draw_header(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
	menu_cell_basic_header_draw(ctx, cell_layer, "Pixel Dogfight");
}
static void menu_draw_row(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  // This draws the cell with the appropriate message
	switch (cell_index->row) {
		case 0:
			menu_cell_basic_draw(ctx, cell_layer, "Start Game!", NULL, NULL);
			break;
		case 1:
			menu_cell_basic_draw(ctx, cell_layer, "Player Attack", fire_modes[p1attack], NULL);
			break;
		case 2:
			menu_cell_basic_draw(ctx, cell_layer, "Enemy Attack", fire_modes[p2attack], NULL);
			break;
		case 3:
			menu_cell_basic_draw(ctx, cell_layer, "Help & Credits", NULL, NULL);
			break;
		default:
			break;
  }
}

static void menu_select(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
	switch (cell_index->row) {
		case 0:
			window_stack_push(main_game_w, true);
			break;
		case 1:
			p1attack = (p1attack + 1) % NUM_FIRE_MODES;
			layer_mark_dirty(menu_layer_get_layer(main_menu_l));
			break;
		default:
			break;
	}
}



void game_window_load(Window *window) {
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_bounds(window_layer);
	//GPoint center = grect_center_point(&bounds);
	
	#ifdef PBL_COLOR
		window_set_background_color(main_menu_w, GColorCeleste);
	#else
  	window_set_background_color(main_menu_w, GColorWhite);
	#endif
	
	bg_bitmap = gbitmap_create_with_resource(RESOURCE_ID_BG_IMAGE);
	bg_layer = bitmap_layer_create(bounds);
	bitmap_layer_set_bitmap(bg_layer, bg_bitmap);
	layer_add_child(window_layer, bitmap_layer_get_layer(bg_layer));
	
	game_layer = layer_create(bounds);
	layer_set_update_proc(game_layer, game_render);
	layer_add_child(window_layer, game_layer);
	
	Player1 = plane_create(1, 20, 50, 0, p1attack);
	bullets = bullets_create(bullets);
	bullet_bmps_init();
	
	game_running = true;
	game_over = false;
	app_timer_register(REFRESH_TIMER, update_game_frame, NULL);
}

void game_window_unload(Window *window) {
	game_running = false;
	game_over = true;
	
	bullet_bmps_deinit();
	bullets_destroy(bullets);
	plane_destroy(Player1);
	
	layer_destroy(game_layer);
	gbitmap_destroy(bg_bitmap);
	bitmap_layer_destroy(bg_layer);
}

void main_menu_load(Window *window) {
	Layer *window_layer = window_get_root_layer(window);

  // Create the menu layer
  main_menu_l = menu_layer_create(layer_get_frame(window_layer));
  menu_layer_set_callbacks(main_menu_l, NULL, (MenuLayerCallbacks){
		.get_num_sections = menu_get_num_sections,
		.get_num_rows = menu_get_num_rows,
    .get_header_height = menu_get_header_height,
    .draw_header = menu_draw_header,
    .draw_row = menu_draw_row,
    .select_click = menu_select
  });

  // Bind the menu layer's click config provider to the window for interactivity
  menu_layer_set_click_config_onto_window(main_menu_l, window);
  layer_add_child(window_layer, menu_layer_get_layer(main_menu_l));
}

void main_menu_unload(Window *window) {
	menu_layer_destroy(main_menu_l);
}

void init() {
	main_menu_w = window_create();
	window_set_window_handlers(main_menu_w, (WindowHandlers) {
    .load = main_menu_load,
    .unload = main_menu_unload
  });
	
	main_game_w = window_create();
	window_set_click_config_provider(main_game_w, click_config_provider);
	window_set_window_handlers(main_game_w, (WindowHandlers) {
    .load = game_window_load,
    .unload = game_window_unload
  });
	
	window_stack_push(main_menu_w, true);
}

void deinit() {
	window_destroy(main_menu_w);
	window_destroy(main_game_w);
}

int main(void) {
	init();
	app_event_loop();
	deinit();
}