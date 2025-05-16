#pragma once

#include <stdint.h>

#define DISPLAY_FRAME_TIME_MS 50

// Update a single pixel in the back buffer
void display_px(uint16_t x, uint16_t y, bool val);

void display_backlight(bool enabled);

void display_size(uint16_t& w, uint16_t& h);

// Initialize display, start background task
void display_init();

// If initialized, stop the display (suspend background task, etc)
void display_suspend();

// Resume after suspend
void display_resume();
