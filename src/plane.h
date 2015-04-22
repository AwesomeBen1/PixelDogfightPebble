#include "pebble.h"

typedef struct {
	// Positions are in tenths of a pixel
	int player, x, y, rot, speed, cdwn;
	int16_t w, h;
	GBitmap* bitmaps[16];
} Plane;

void plane_render(Plane *plane, GContext *ctx);
void plane_move(Plane *plane);
void update_bitmaps(Plane *plane);
Plane *plane_create(int player, int x, int y, int rot);
void plane_destroy(Plane *plane);