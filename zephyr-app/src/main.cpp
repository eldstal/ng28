#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(ng28, LOG_LEVEL_INF);

#include <zephyr/sys/printk.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/display.h>



const struct device *display_dev;
struct display_capabilities display_caps;

struct display_buffer_descriptor framebuf_desc;
uint8_t* framebuf;

inline void sim_display_px(size_t x, size_t y, bool val) {
    size_t bit = y * framebuf_desc.width + x;

    size_t byte = bit / 8;
    size_t bitoff = bit % 8;

    // Out of bounds
    if (byte >= framebuf_desc.buf_size) return;

    if (val) {
        framebuf[byte] |= 1<<bitoff;
    } else {
        framebuf[byte] &= 0xFF^(1<<bitoff);
    }
}

inline void sim_display_flip() {
    display_write(display_dev, 0, 0, &framebuf_desc, framebuf);
}

int sim_display_setup() {

    display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));

    if (!device_is_ready(display_dev)) {
        LOG_ERR("Device %s is not found/ready. Won't be displaying anything.", display_dev->name);
        return -1;
    }

    display_get_capabilities(display_dev, &display_caps);


    framebuf_desc.buf_size = display_caps.x_resolution * display_caps.y_resolution;
    framebuf_desc.buf_size = DIV_ROUND_UP(framebuf_desc.buf_size, 8);

    framebuf_desc.pitch = display_caps.x_resolution;
    framebuf_desc.width = display_caps.x_resolution;
    framebuf_desc.height = display_caps.y_resolution;
    framebuf_desc.frame_incomplete = false;


    LOG_INF("Framebuffer: %lu bytes", framebuf_desc.buf_size);
    framebuf = (uint8_t*) k_malloc(framebuf_desc.buf_size);

    if (!framebuf) {
        LOG_ERR("Failed to allocate framebuffer.");
        return -1;
    }



    return 0;

}



int main() {


    if (sim_display_setup()) return 1;

    LOG_INF("OK, we're up!\n");

    for (size_t p=0; p<40; p++) sim_display_px(p, p, true);

    sim_display_flip();



}
