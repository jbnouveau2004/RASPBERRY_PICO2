#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"

#include "pico/cyw43_arch.h"

#include "lwip/pbuf.h"
#include "lwip/tcp.h"

#define WIFI_SSID "TP-Link_2B38_EXT"
#define WIFI_PASSWORD "20947170"

#define BUTTON_PIN 3
#define LED_PIN 2

#define vanne2_PIN 4
bool vanne2_state = false;

#define ADC_PIN 26
#define ADC_INPUT 0

#define PWM_PIN 5

float read_adc_voltage(void)
{
    adc_select_input(ADC_INPUT);
    uint16_t raw = adc_read();   // valeur entre 0 et 4095
    float voltage = raw * 3.3f / 4095.0f;
    return voltage;
}

static err_t http_callback(void *arg,
                           struct tcp_pcb *tpcb,
                           struct pbuf *p,
                           err_t err)
{
    if (!p) {
        tcp_close(tpcb);
        return ERR_OK;
    }

    char request[256];

    int len = p->tot_len;
    if (len > 255) {
        len = 255;
    }

    pbuf_copy_partial(p, request, len, 0);
    request[len] = '\0';

    tcp_recved(tpcb, p->tot_len);
    pbuf_free(p);


    // Route GPIO
    if (strstr(request, "GET /gpio") != NULL) {

        char response[256];

        int state = gpio_get(BUTTON_PIN);

        sprintf(response,
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Cache-Control: no-cache\r\n"
            "Connection: close\r\n"
            "\r\n"
            "%d",
            state);

        tcp_write(tpcb, response, strlen(response), TCP_WRITE_FLAG_COPY);
        tcp_output(tpcb);
        tcp_close(tpcb);

        return ERR_OK;
    }

    // Route GPIO
    if (strstr(request, "GET /togglevanne2") != NULL) {

        vanne2_state = gpio_get(vanne2_PIN);
        vanne2_state = !vanne2_state;

        gpio_put(vanne2_PIN, vanne2_state);

        const char *response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Connection: close\r\n"
            "\r\n"
            "OK";

        tcp_write(tpcb,
                response,
                strlen(response),
                TCP_WRITE_FLAG_COPY);

        tcp_output(tpcb);
        tcp_close(tpcb);

        return ERR_OK;
    }

    // Route GPIO
    if (strstr(request, "GET /voltage") != NULL) {
        float voltage = read_adc_voltage();

        char body[64];
        sprintf(body, "%.2f", voltage);

        char response[256];

        sprintf(response,
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: text/plain\r\n"
                "Cache-Control: no-cache\r\n"
                "Connection: close\r\n"
                "Content-Length: %d\r\n"
                "\r\n"
                "%s",
                (int)strlen(body),
                body);

        tcp_write(tpcb, response, strlen(response), TCP_WRITE_FLAG_COPY);
        tcp_output(tpcb);
        tcp_close(tpcb);

        return ERR_OK;
    }

    //route GPIO
    if (strstr(request, "GET /pwm") != NULL) {

        float voltage = 0.0f;

        char *v = strstr(request, "v=");
        if (v != NULL) {
            voltage = atof(v + 2);
        }

        if (voltage < 0.0f) {
            voltage = 0.0f;
        }

        if (voltage > 3.3f) {
            voltage = 3.3f;
        }

        uint slice_num = pwm_gpio_to_slice_num(PWM_PIN);
        uint channel = pwm_gpio_to_channel(PWM_PIN);
        uint16_t level = (uint16_t)(voltage * 4095.0f / 3.3f);

        pwm_set_chan_level(slice_num, channel, level);

        char response[256];

        sprintf(response,
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Connection: close\r\n"
            "\r\n"
            "PWM %.2f V",
            voltage);

        tcp_write(tpcb, response, strlen(response), TCP_WRITE_FLAG_COPY);
        tcp_output(tpcb);
        tcp_close(tpcb);

        return ERR_OK;
    }

    // Page principale
    else {

        const char *html =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "Connection: close\r\n"
            "\r\n"
            "<!DOCTYPE html>"
            "<html>"
            "<head><meta charset='UTF-8'><title>Pico 2 W</title></head>"

            "<style>"
            ".vert{background-color:green;}"
            ".rouge{background-color:red;}"
            ".vert, .rouge{width:50px;height:50px;border-radius:25px 25px;}"
            "</style>"

            "<body>"
            "<h1>Bonjour depuis le Pico 2 WH !</h1>"
            "<p>Vanne1=PB2 / Vanne2=PB4 / Tension(entrée)=PB26 / Tension(sortie)=PB5</p>"
            "<div id='gpio'>...</div>"

            "<button class='btn'>Vanne 2</button>"


            "<div id='voltage'>...</div>"


            "<button class='btn1'>0.0v</button>"
            "<button class='btn2'>0.5v</button>"
            "<button class='btn3'>1.0v</button>"
            "<button class='btn4'>1.5v</button>"
            "<button class='btn5'>2.0v</button>"
            "<button class='btn6'>2.5v</button>"
            "<button class='btn7'>3.0v</button>"
            "<button class='btn8'>3.3v</button>"

            "<input id='input' type='range' min='0' max='3.3' step='0.1' />"

            "<script>"


            "setInterval(async () => {"

            " let r = await fetch('/gpio');"
            " let t = await r.text();"
            " if(t=='1'){document.getElementById('gpio').innerHTML = `<div>La vanne 1 est ouverte</div><div class='vert'></div>`;}"
            " else{document.getElementById('gpio').innerHTML = `<div>La vanne 1 est fermée</div><div class='rouge'></div>`;}"

            "        let y = await fetch('/voltage');"
            "        let z = await y.text();"
            "        document.getElementById('voltage').innerText = z + ' V';"  

            "}, 200);"
            
            " btn=document.querySelector('.btn');"
            " btn.addEventListener('click', toggleVanne2);"
            " async function toggleVanne2() {"
            " await fetch('/togglevanne2');}"

            " btn=document.querySelector('.btn1');"
            " btn.addEventListener('click', tension1);"
            " async function tension1() {"
            " await fetch('/pwm?v=0.0');}"
            " btn=document.querySelector('.btn2');"
            " btn.addEventListener('click', tension2);"
            " async function tension2() {"
            " await fetch('/pwm?v=0.5');}"
            " btn=document.querySelector('.btn3');"
            " btn.addEventListener('click', tension3);"
            " async function tension3() {"
            " await fetch('/pwm?v=1.0');}"
            " btn=document.querySelector('.btn4');"
            " btn.addEventListener('click', tension4);"
            " async function tension4() {"
            " await fetch('/pwm?v=1.5');}"
            " btn=document.querySelector('.btn5');"
            " btn.addEventListener('click', tension5);"
            " async function tension5() {"
            " await fetch('/pwm?v=2.0');}"
            " btn=document.querySelector('.btn6');"
            " btn.addEventListener('click', tension6);"
            " async function tension6() {"
            " await fetch('/pwm?v=2.5');}"
            " btn=document.querySelector('.btn7');"
            " btn.addEventListener('click', tension7);"
            " async function tension7() {"
            " await fetch('/pwm?v=3.0');}"
            " btn=document.querySelector('.btn8');"
            " btn.addEventListener('click', tension8);"
            " async function tension8() {"
            " await fetch('/pwm?v=3.3');}"

            "const input = document.querySelector('#input');"
            "input.addEventListener('input', async (event) => {await fetch('/pwm?v=' + event.target.value);});"
            
            "</script>"

            "</body>"
            "</html>";

        tcp_write(tpcb, html, strlen(html), TCP_WRITE_FLAG_COPY);
        tcp_output(tpcb);
        tcp_close(tpcb);

        return ERR_OK;
    }
}

static err_t connection_callback(void *arg,
                                 struct tcp_pcb *newpcb,
                                 err_t err)
{
    tcp_recv(newpcb, http_callback);
    return ERR_OK;
}

int main()
{

    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);

    // Active la résistance de pull-up interne
    gpio_pull_up(BUTTON_PIN);

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    gpio_init(vanne2_PIN);
    gpio_set_dir(vanne2_PIN, GPIO_OUT);

    adc_init();
    adc_gpio_init(ADC_PIN);

    gpio_set_function(PWM_PIN, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(PWM_PIN);
    uint channel = pwm_gpio_to_channel(PWM_PIN);
    pwm_set_wrap(slice_num, 4095);
    pwm_set_chan_level(slice_num, channel, 0);
    pwm_set_enabled(slice_num, true);


    stdio_init_all();

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
            30000))
    {
        printf("Echec connexion\n");
        return -1;
    }

    printf("WiFi connecté\n");
    printf("IP: %s\n", ip4addr_ntoa(netif_ip4_addr(netif_list)));

    struct tcp_pcb *pcb = tcp_new();

    if (!pcb) {
        printf("Erreur TCP\n");
        return -1;
    }

    if (tcp_bind(pcb, IP_ADDR_ANY, 80) != ERR_OK) {
        printf("Erreur bind\n");
        return -1;
    }

    pcb = tcp_listen_with_backlog(pcb, 1);

    tcp_accept(pcb, connection_callback);

    printf("Serveur HTTP lancé\n");

    while (true) {

        // Bouton appuyé = 0 avec pull-up
        if (gpio_get(BUTTON_PIN) == 0) {

            gpio_put(LED_PIN, 1);
            printf("Vanne ouverte\n");

        } else {

            gpio_put(LED_PIN, 0);
            printf("Vanne fermée\n");
        }

        sleep_ms(1000);

    }

    cyw43_arch_deinit();

    return 0;
}