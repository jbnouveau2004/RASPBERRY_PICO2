#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "../wifi/wifi.h"
#include "../hardware/hardware.h"
#include "../server/server.h"

#define BUTTON_PIN 3
#define LED_PIN 2



int main()
{
    stdio_init_all();

    sleep_ms(3000);

    hardware_init();

    if (wifi_connect() != 0) {
        return -1;
    }

    if (http_server_start() != 0) {
        return -1;
    }

    if (https_server_start() != 0) {
        return -1;
    }


    while (true) {

        // boutton appuyé (0v) alors led à 1 sinon led à 0
        gpio_put(LED_PIN, gpio_get(BUTTON_PIN) == 0 ? 1 : 0);

        cyw43_arch_poll();
        sleep_ms(1);

    }


}