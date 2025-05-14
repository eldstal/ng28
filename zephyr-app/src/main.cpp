#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(ng28, LOG_LEVEL_INF);

#include <zephyr/sys/printk.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/display.h>


#define PX_FG ((uint32_t) 0xFF183747)
#define PX_BG ((uint32_t) 0xFFa1a595)       // Backlight off
#define PX_BL ((uint32_t) 0xFF8FF7FC)       // Backlight on

uint32_t px_on = PX_FG;
uint32_t px_off = PX_BG;

const struct device *display_dev;
struct display_capabilities display_caps;

struct display_buffer_descriptor framebuf_desc;
uint32_t* framebuf;

inline void sim_display_px(size_t x, size_t y, bool val) {

    size_t pos = display_caps.x_resolution * y + x;

    // Out of bounds
    if (pos >= framebuf_desc.buf_size/4) return;

    if (val) {
        framebuf[pos] = px_on;
    } else {
        framebuf[pos] = px_off;
    }
}

inline void sim_display_flip() {
    display_write(display_dev, 0, 0, &framebuf_desc, framebuf);
}

inline void sim_display_backlight(bool enabled) {
    
    px_off = enabled ? PX_BL : PX_BG;
    
    for (size_t i=0; i < display_caps.x_resolution * display_caps.y_resolution; i++) {
        framebuf[i] = framebuf[i] == px_on ? px_on : px_off;
    }

    sim_display_flip();
}

int sim_display_setup() {

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


    LOG_INF("Framebuffer: %lu bytes", framebuf_desc.buf_size);
    framebuf = (uint32_t*) k_malloc(framebuf_desc.buf_size);

    if (!framebuf) {
        LOG_ERR("Failed to allocate framebuffer.");
        return -1;
    }

    display_blanking_off(display_dev);


    return 0;

}



int main() {


    if (sim_display_setup()) return 1;

    LOG_INF("OK, we're up!\n");

    bool mode = false;
    while (true) {
        for (size_t y=0; y<display_caps.y_resolution; y++) {
            for (size_t x=0; x<display_caps.x_resolution; x++) {
                sim_display_px(x, y, ((x+y)%2));
            }
            sim_display_flip();
            k_sleep(K_MSEC(1));
        }

        mode = !mode;
        sim_display_backlight(mode);
    }



}
