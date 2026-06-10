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

        // test si perte de connexion Wifi puis reconnexion si jamais
//        wifi_reconnect_if_needed();




/** static int bad_wifi_count = 0;

int status = cyw43_wifi_link_status(&cyw43_state, CYW43_ITF_STA);

if (status != CYW43_LINK_UP) {
    bad_wifi_count++;
    printf("status = %d\n", status);
} else {
    bad_wifi_count = 0;
}

if (bad_wifi_count > 10) {
    reconnect_wifi();
    bad_wifi_count = 0;
}

        sleep_ms(5000);
*/
    }


}