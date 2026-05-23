#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "lwip/netif.h"
#include "lwip/ip4_addr.h"

#include "wifi.h"

#define WIFI_SSID "TP-Link_2B38"
#define WIFI_PASSWORD "20947170"

int wifi_connect(void)
{
    if (cyw43_arch_init()) {
        printf("Erreur WiFi\n");
        return -1;
    }

    cyw43_arch_enable_sta_mode();

    printf("Connexion WiFi...\n");

    // pour une visibilité sur le routeur
    netif_set_hostname(netif_default, "pico-server");

    // imposer une adresse IP hors plage et réservation DHCP
    ip4_addr_t ip, mask, gw;
    IP4_ADDR(&ip,   192,168,1,20);
    IP4_ADDR(&mask, 255,255,255,0);
    IP4_ADDR(&gw,   192,168,1,1);
    dhcp_stop(netif_default);
    netif_set_addr(netif_default,
               &ip,
               &mask,
               &gw);

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

    // affiche l'adresse MAC
    uint8_t mac[6];
    cyw43_wifi_get_mac(&cyw43_state, CYW43_ITF_STA, mac);
    printf("MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
        mac[0], mac[1], mac[2],
        mac[3], mac[4], mac[5]);

    return 0;
}