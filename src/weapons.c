#include "pebble.h"
#include "weapons.h"

#define BULLET_SPEED 60

static uint16_t bullet_list_len, bullets_active;
static GBitmap *bullet_bitmaps[2];

void bullets_destroy(Bullet * bullets) {
	free(bullets);
}

void bullets_render(Bullet * bullets, GContext *ctx) {
	for (int i = 0; i < bullets_active; ++i) {
		graphics_draw_bitmap_in_rect(ctx, bullet_bitmaps[bullets[i].player - 1], (GRect) {.origin = {(bullets[i].x / 10 - 1), (bullets[i].y / 10 - 1)}, .size = {3, 3}});
	}
}

void bullets_move(Bullet * bullets) {
	for (int i = bullets_active - 1; i >= 0; --i) {
		bullets[i].x += cos_lookup(bullets[i].angle) * BULLET_SPEED / TRIG_MAX_RATIO;
		bullets[i].y -= sin_lookup(bullets[i].angle) * BULLET_SPEED / TRIG_MAX_RATIO;
		// Optimize later
		if (bullets[i].y / 10 > 106 || bullets[i].y / 10 < -3) {
			bullets = bullet_destroy(bullets, i);
		}
		if (bullets[i].x / 10 > 144) {
			if (bullets[i].looped) {
				bullets = bullet_destroy(bullets, i);
			} else {
				bullets[i].looped = true;
				bullets[i].x = -3 * 10;
			}
		} else if (bullets[i].x / 10 < -3) {
			if (bullets[i].looped) {
				bullets = bullet_destroy(bullets, i);
			} else {
				bullets[i].looped = true;
				bullets[i].x = 144 * 10;
			}
		}
	}
}

Bullet *bullets_create(Bullet * bullets) {
	bullets_active = 0;
	bullet_list_len = 1;
	bullets = (Bullet *) malloc(sizeof(Bullet));
	return bullets;
}

Bullet *bullet_create(Bullet * bullets, int player, int x, int y, int rot) {
	++bullets_active;
	if (bullets_active > bullet_list_len) {
		++bullet_list_len;
		void* temp = realloc(bullets, bullets_active * sizeof(Bullet));
		if(!temp) {
			APP_LOG(APP_LOG_LEVEL_ERROR, "Error: Can't allocate memory for bullets array.");
			bullets_destroy(bullets);
			return NULL;
		}
		bullets = temp;
	}
	int aindex = bullets_active - 1;
	bullets[aindex].player = player;
	bullets[aindex].x = x;
	bullets[aindex].y = y;
	bullets[aindex].angle = rot * TRIG_MAX_ANGLE / 16;
	bullets[aindex].looped = false;
	return bullets;
}

Bullet *bullet_destroy(Bullet * bullets, int aindex) {
	for (int i = aindex; i < bullets_active - aindex - 1; ++i) {
		bullets[i] = bullets[i + 1];
	}
	--bullets_active;
	return bullets;
}

void bullet_bmps_init(void) {
	bullet_bitmaps[0] = gbitmap_create_with_resource(RESOURCE_ID_P1_BULLET);
	bullet_bitmaps[1] = gbitmap_create_with_resource(RESOURCE_ID_P2_BULLET);
}

void bullet_bmps_deinit(void) {
	gbitmap_destroy(bullet_bitmaps[0]);
	gbitmap_destroy(bullet_bitmaps[1]);
}