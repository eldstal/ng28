#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(ng28, LOG_LEVEL_INF);

#include <zephyr/kernel.h>

#include "viewmanager.hpp"
#include "display.hpp"

int main() {


    display_init();

    uint16_t w, h;
    display_size(w, h);

    vman_start();

    LOG_INF("OK, we're up!\n");
    while (true) {
        k_sleep(K_MSEC(1000));
    }


    bool mode = false;
    while (true) {
        for (size_t y=0; y<h; y++) {
            for (size_t x=0; x<w; x++) {
                display_px(x, y, mode != ((x+y)%2));
            }
        }

        k_sleep(K_MSEC(1));
        mode = !mode;
        //display_backlight(mode);
    }



}
