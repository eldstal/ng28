#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(ng28, LOG_LEVEL_INF);

#include <zephyr/kernel.h>

#include "viewmanager.hpp"
#include "display.hpp"

int main() {


    display_init();
    display_backlight(false);

    uint16_t w, h;
    display_size(w, h);

    vman_start();

    LOG_INF("OK, we're up!\n");
    while (true) {
        k_sleep(K_MSEC(1000));
    }




}
