#include <pebble.h>

typedef struct {
	GBitmap *bitmap;
	// Positions are in hundredths of a pixel
	int x, y, angle, mcdwn, scdwn;
} Plane;

static GBitmap* {};

static Plane *plane_create(int x, int y, int rot, GBitmap *bitmap) {
	Plane *new_plane = malloc(sizeof(Plane));
	new_plane->x = x * 100;
	new_plane->y = y * 100;
	new_plane->rot = rot;
	new_plane->mcdwn = 20;
	new_plane->scdwn = 100;
	new_plane->bitmap = bitmap;
	return new_plane;
}

static void *player1_render(GContext *ctx) {
	switch Plane1->rot {
		case 0:
			graphics_draw_bitmap_in_rect(ctx, plane1, (GRect) {.origin = {(Player1->x / 100), (Player1->y / 100)}, .size = {15, 12}});
		case 1:
			graphics_draw_bitmap_in_rect(ctx, plane1, (GRect) {.origin = {(Player1->x / 100), (Player1->y / 100)}, .size = {15, 12}});
}
	
static void *player1_move(void) {
	Player1->x += 100;
	// Optimize later
	if (Player1->x / 100 > 144 - 5) {
		Player1->x = -5;
	}
}

static void *create_bitmaps(void) {
	s0_deg = gbitmap_create_with_resource(RESOURCE_ID_PLANE1_R1);
}