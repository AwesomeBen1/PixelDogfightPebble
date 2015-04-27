#include "pebble.h"

typedef struct {
	// Positions are in tenths of a pixel
	int x, y;
	int8_t player;
	int32_t angle;
	bool looped;
	GBitmap *bitmap;
} Bullet;

void bullets_destroy(Bullet * bullets);
void bullets_render(Bullet * bullets, GContext *ctx);
void bullets_move(Bullet * bullets);
Bullet *bullets_create(Bullet * bullets);
Bullet *bullet_create(Bullet * bullets, int player, int x, int y, int rot);
Bullet *bullet_destroy(Bullet * bullets, int aindex);
void bullet_bmps_init(void);
void bullet_bmps_deinit(void);