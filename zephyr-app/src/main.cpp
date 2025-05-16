#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(ng28, LOG_LEVEL_INF);

#include <zephyr/kernel.h>

#include "display.hpp"

int main() {


    display_init();

    uint16_t w, h;
    display_size(w, h);

    LOG_INF("OK, we're up!\n");

    bool mode = false;
    while (true) {
        for (size_t y=0; y<h; y++) {
            for (size_t x=0; x<w; x++) {
                display_px(x, y, ((x+y)%2));
            }
        }

        k_sleep(K_MSEC(1));
        mode = !mode;
        display_backlight(mode);
    }



}
