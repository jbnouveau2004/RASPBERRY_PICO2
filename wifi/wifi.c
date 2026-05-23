#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "lwip/netif.h"
#include "lwip/ip4_addr.h"

#include "wifi.h"

#define WIFI_SSID "TP-Link_2B38_EXT"
#define WIFI_PASSWORD "20947170"

int wifi_connect(void)
{
    if (cyw43_arch_init()) {
        printf("Erreur WiFi\n");
        return -1;
    }

    cyw43_arch_enable_sta_mode();

    printf("Connexion WiFi...\n");

    if (cyw43_arch_wifi_connect_timeout_ms(
            WIFI_SSID,
            WIFI_PASSWORD,
            CYW43_AUTH_WPA2_AES_PSK,
            30000)) {
        printf("Echec connexion\n");
        return -1;
    }

    printf("WiFi connecté\n");
    printf("IP: %s\n", ip4addr_ntoa(netif_ip4_addr(netif_default)));

    return 0;
}