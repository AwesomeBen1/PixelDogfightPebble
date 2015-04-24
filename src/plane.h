#include "pebble.h"

#define PLANE_SPEED 20
#define FIRE_RATE 20

typedef struct {
	// Positions are in tenths of a pixel
	int x, y, speed, cdwn, firerate;
	int8_t player, rot, burstrepeat;
	int16_t w, h;
	bool crashed;
	GBitmap* bitmaps[16];
	GBitmap* boom;
} Plane;

void plane_render(Plane *plane, GContext *ctx);
void plane_move(Plane *plane);
void update_bitmaps(Plane *plane);
Plane *plane_create(int player, int x, int y, int rot, int attack);
void plane_destroy(Plane *plane);