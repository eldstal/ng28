#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(display_sim, LOG_LEVEL_INF);

#include <zephyr/sys/printk.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/display.h>

#include "config.hpp"
#include "display.hpp"

#define PX_FG ((uint32_t) 0xFF183747)
#define PX_BG ((uint32_t) 0xFFa1a595)       // Backlight off
#define PX_BL ((uint32_t) 0xFF8FF7FC)       // Backlight on

struct k_mutex framebuf_mutex;

uint32_t px_on = PX_FG;
uint32_t px_off = PX_BG;

const struct device *display_dev;
struct display_capabilities display_caps;

struct display_buffer_descriptor framebuf_desc;
uint32_t* framebuf;
bool framebuf_dirty;

static inline void sim_display_px(uint16_t x, uint16_t y, bool val) {

    size_t pos = display_caps.x_resolution * y + x;

    // Out of bounds
    if (pos >= framebuf_desc.buf_size/4) return;

    framebuf[pos] = val ? px_on : px_off;
}

inline void sim_display_push() {
    display_write(display_dev, 0, 0, &framebuf_desc, framebuf);
}

static inline void sim_display_backlight(bool enabled) {
    
    px_off = enabled ? PX_BL : PX_BG;
    
    for (size_t i=0; i < display_caps.x_resolution * display_caps.y_resolution; i++) {
        framebuf[i] = framebuf[i] == px_on ? px_on : px_off;
    }

    framebuf_dirty = true;
}

static int sim_display_setup() {

    display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));

    if (!device_is_ready(display_dev)) {
        LOG_ERR("Device %s is not found/ready. Won't be displaying anything.", display_dev->name);
        return -1;
    }

    display_get_capabilities(display_dev, &display_caps);



    framebuf_desc.buf_size = display_caps.x_resolution * display_caps.y_resolution * 4;

    framebuf_desc.pitch = display_caps.x_resolution;
    framebuf_desc.width = display_caps.x_resolution;
    framebuf_desc.height = display_caps.y_resolution;
    framebuf_desc.frame_incomplete = false;

    k_mutex_init(&framebuf_mutex);

    k_mutex_lock(&framebuf_mutex, K_FOREVER); {

        LOG_INF("Framebuffer: %lu bytes", framebuf_desc.buf_size);
        framebuf = (uint32_t*) k_malloc(framebuf_desc.buf_size);

        if (!framebuf) {
            LOG_ERR("Failed to allocate framebuffer.");
            return -1;
        }

        sim_display_backlight(false);
        sim_display_push();
    }


    k_mutex_unlock(&framebuf_mutex);

    display_blanking_off(display_dev);


    return 0;

}


void sim_display_thread(void* d0, void* d1, void* d2) {

    while (true) {
        k_mutex_lock(&framebuf_mutex, K_FOREVER);
            if (framebuf_dirty) {
                sim_display_push();
                framebuf_dirty = false;
            }
        k_mutex_unlock(&framebuf_mutex);

        // TODO: Subtract the duration of this last iteration
        k_sleep(K_MSEC(DISPLAY_FRAME_TIME_MS));
    }
}

K_THREAD_DEFINE(sim_display_tid, 512,
               sim_display_thread, NULL, NULL, NULL,
               K_LOWEST_APPLICATION_THREAD_PRIO, 0, 0);

/*
 * Exposed interface
 */

void display_px(uint16_t x, uint16_t y, bool val) {
    k_mutex_lock(&framebuf_mutex, K_FOREVER);
        sim_display_px(x, y, val);
#if DISPLAY_DOUBLE_BUFFER!=1
        framebuf_dirty = true;
#endif
    k_mutex_unlock(&framebuf_mutex);
}

void display_fill(bool val) {
    k_mutex_lock(&framebuf_mutex, K_FOREVER);
        for (size_t x=0; x<display_caps.x_resolution; x++) {
            for (size_t y=0; y<display_caps.x_resolution; y++) {
                sim_display_px(x, y, val);
            }
        }

#if DISPLAY_DOUBLE_BUFFER!=1
        framebuf_dirty = true;
#endif
    k_mutex_unlock(&framebuf_mutex);
}

void display_flip() {
#if DISPLAY_DOUBLE_BUFFER==1
    k_mutex_lock(&framebuf_mutex, K_FOREVER);
        framebuf_dirty = true;
    k_mutex_unlock(&framebuf_mutex);
#endif
}

void display_backlight(bool enabled) {
    k_mutex_lock(&framebuf_mutex, K_FOREVER);
        sim_display_backlight(enabled);
    k_mutex_unlock(&framebuf_mutex);
}

void display_size(uint16_t& w, uint16_t& h) {
    w = display_caps.x_resolution;
    h = display_caps.y_resolution;
}

void display_init() {
    sim_display_setup();
    k_thread_start(sim_display_tid);
}
