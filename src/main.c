// Pixel Dogfight for Pebble
// Made by Ben Chapman-Kish from 2015-04-09 to 2015-09-10
#include <pebble.h>
	
#define TIMER_MS 50

typedef struct {
	GBitmap *image;
	// Positions and angle are in hundredths of a pixel
	int x, y, angle, mcdwn, scdwn;
} Plane;

static Window *main_menu_w, *main_game_w;
static MenuLayer *main_menu_l;
static Layer *game_layer;
static GBitmap *bg_bitmap, *p1_bitmap/*, *p2_bitmap*/;
static BitmapLayer *bg_layer;
//static RotBitmapLayer *p1_layer/*, *p2_layer*/;
//static int p1attack = 1, p2attack = 1;
static Plane *Player1/*, *Player2*/;



static Plane *plane_create(int x, int y, int angle) {
	Plane *new_plane = malloc(sizeof(Plane));
	new_plane->x = x * 100;
	new_plane->y = y * 100;
	new_plane->angle = angle * 100;
	new_plane->mcdwn = 20;
	new_plane->scdwn = 100;
	return new_plane;
}


void game_render(Layer *layer, GContext *ctx) {
	graphics_draw_bitmap_in_rect(ctx, p1_bitmap, (GRect) {.origin = {Player1->x, Player1->y}, .size = {15, 12}});
	//graphics_draw_bitmap_in_rect(ctx, p2_bitmap, (GRect) {.origin = {Player2->x, Player2->y}, .size = {15, 12}});
}

static void update_game_frame(void *data) {
	Player1->x += 10;
	if (Player1->x > 144 - 7) {
		Player1->x = 0;
	}
	app_timer_register(TIMER_MS, update_game_frame, NULL);
	layer_mark_dirty(game_layer);
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
			menu_cell_basic_draw(ctx, cell_layer, "Player Attack", "Bullet", NULL);
			break;
		case 2:
			menu_cell_basic_draw(ctx, cell_layer, "Enemy Attack", "Bullet", NULL);
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
	
	Player1 = plane_create(20, 40, 0);
	//Player2 = plane_create(144-20, 40, 180);
	
	layer_set_update_proc(game_layer, game_render);
	app_timer_register(TIMER_MS, update_game_frame, NULL);
}

void game_window_unload(Window *window) {
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
	
}

void init() {
	main_menu_w = window_create();
	window_set_window_handlers(main_menu_w, (WindowHandlers) {
    .load = main_menu_load,
    .unload = main_menu_unload
  });
	
	main_game_w = window_create();
	window_set_window_handlers(main_game_w, (WindowHandlers) {
    .load = game_window_load,
    .unload = game_window_unload
  });
	
	window_stack_push(main_menu_w, true);
}

void deinit() {
	window_destroy(main_menu_w);
}

int main(void) {
	init();
	app_event_loop();
	deinit();
}