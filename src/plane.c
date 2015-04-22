#include "pebble.h"

typedef struct {
	// Positions are in tenths of a pixel
	int player, x, y, rot, speed, cdwn;
	GBitmap* bitmaps[16];
} Plane;

Plane *plane_create(int player, int x, int y, int rot, GBitmap *bitmap) {
	Plane *new_plane = malloc(sizeof(Plane));
	new_plane->player = player;
	new_plane->x = x * 10;
	new_plane->y = y * 10;
	new_plane->rot = rot;
	new_plane->speed = 10;
	new_plane->cdwn = 20;
	if (player == 1) {
		new_plane->bitmaps[0] = gbitmap_create_with_resource(RESOURCE_ID_PLANE1_R0);
		new_plane->bitmaps[1] = gbitmap_create_with_resource(RESOURCE_ID_PLANE1_R1);
		new_plane->bitmaps[2] = gbitmap_create_with_resource(RESOURCE_ID_PLANE1_R2);
		new_plane->bitmaps[3] = gbitmap_create_with_resource(RESOURCE_ID_PLANE1_R3);
	} else {
		
	}
	return new_plane;
}

void plane_delete(Plane *plane) {
  free(plane);
}

void *plane_render(Plane *plane, GContext *ctx) {
	graphics_draw_bitmap_in_rect(ctx, plane->bitmaps[plane->rot], (GRect) {.origin = {(plane->x / 10), (plane->y / 10)}, .size = {15, 12}});
}

void *plane_move(Plane *plane) {
	int32_t plane_angle = plane->rot * TRIG_MAX_ANGLE / 16;
	plane->x += (-cos_lookup(plane_angle) * plane->speed / TRIG_MAX_RATIO);
	plane->y += (sin_lookup(plane_angle) * plane->speed / TRIG_MAX_RATIO);
	// Optimize later
	if (plane->x / 10 > 144 - 5) {
		plane->x = -50;
	} else if (plane->x / 10 < -5) {
		plane->x = (144 + 5) * 10;
	}
}