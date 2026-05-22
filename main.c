#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"

#include "pico/cyw43_arch.h"

#include "lwip/netif.h"
#include "lwip/ip4_addr.h"
#include "lwip/altcp.h"
#include "lwip/altcp_tls.h"
#include "lwip/pbuf.h"

#define WIFI_SSID "TP-Link_2B38_EXT"
#define WIFI_PASSWORD "20947170"

#define BUTTON_PIN 3
#define LED_PIN 2

#define vanne2_PIN 4
bool vanne2_state = false;

#define ADC_PIN 26
#define ADC_INPUT 0

#define PWM_PIN 5

static const unsigned char server_cert[] =
"-----BEGIN CERTIFICATE-----\n"
"MIIDCzCCAfOgAwIBAgIUHPMmlfgwnKh30Z3YcVx4d/tWwgAwDQYJKoZIhvcNAQEL\n"
"BQAwFTETMBEGA1UEAwwKcGljby5sb2NhbDAeFw0yNjA1MjEyMjM4MDNaFw0yNzA1\n"
"MjEyMjM4MDNaMBUxEzARBgNVBAMMCnBpY28ubG9jYWwwggEiMA0GCSqGSIb3DQEB\n"
"AQUAA4IBDwAwggEKAoIBAQDU7R9czc3MbprNL01C0UB2kl+DyE27XpJVFe8Bm5HP\n"
"MYrb1LZ1s1B1lXcCsWsMmyntz1euizM0swcezrUYL4Q2rGzEOstetcIMO/vgtpx3\n"
"cbSPoqVN7pk1kAqiHTrAP2IFC+7p1MXqNjEfkHTDqrhTS++CVDnv8evQMvdF2OT3\n"
"nFFcJp2uObLJHFoIJ26K4mVo2G6s8nOeb9QGD9snkgUBdhANw7rinRoQr4bYjMRW\n"
"ljndX/O3Bxavs9z0GX9yqjn8ZLCRSQTDkmjd5+dfeVaUtND3G7hJphVUJOu3g5P1\n"
"qwM3ziMPd1OAiGUYLYY1zDw9MWnd7TXWtAYYBtWU/QTxAgMBAAGjUzBRMB0GA1Ud\n"
"DgQWBBSXYV7R5pnYzc0Bz2qGUIDM3pmV5zAfBgNVHSMEGDAWgBSXYV7R5pnYzc0B\n"
"z2qGUIDM3pmV5zAPBgNVHRMBAf8EBTADAQH/MA0GCSqGSIb3DQEBCwUAA4IBAQBu\n"
"flqpcdHCIsJQsGnKGmnMBkbQdaBtLTNSJrc5DD7srGwa6DBjB4XZHURyjWdVv2s4\n"
"Gp129JVmp8v6ayuiuWj2/5cEL8qYGqquO9U5xh+aqgxCVNiAdNrZkfjWc0MT2huH\n"
"z4qaY2LdhijsgbpaYh7TvURFQ1neL27yqAE+HQwwPuhTLT+FvzLF32tuCNz8Dal5\n"
"QRi56uk+a/GSGcXoxFeMtNAOHyGxx9jHY1n3JXkdRX8pHbzWFKJbXhqMu0WVIsXP\n"
"MgWluyrue5R/pT2FSREFSlIYI9YaseSNDwHgJVXP0AJ/gaCk+hzrOSLNq1INfqcs\n"
"AqLIo5jdvR1UfyoEQSSE\n"
"-----END CERTIFICATE-----\n";


static const unsigned char server_key[] =
"-----BEGIN PRIVATE KEY-----\n"
"MIIEvQIBADANBgkqhkiG9w0BAQEFAASCBKcwggSjAgEAAoIBAQDU7R9czc3MbprN\n"
"L01C0UB2kl+DyE27XpJVFe8Bm5HPMYrb1LZ1s1B1lXcCsWsMmyntz1euizM0swce\n"
"zrUYL4Q2rGzEOstetcIMO/vgtpx3cbSPoqVN7pk1kAqiHTrAP2IFC+7p1MXqNjEf\n"
"kHTDqrhTS++CVDnv8evQMvdF2OT3nFFcJp2uObLJHFoIJ26K4mVo2G6s8nOeb9QG\n"
"D9snkgUBdhANw7rinRoQr4bYjMRWljndX/O3Bxavs9z0GX9yqjn8ZLCRSQTDkmjd\n"
"5+dfeVaUtND3G7hJphVUJOu3g5P1qwM3ziMPd1OAiGUYLYY1zDw9MWnd7TXWtAYY\n"
"BtWU/QTxAgMBAAECggEAFZIi1+oEQVkAgcJFrNE6G0kKOCA2D8IC8m+9vjHz0ZkO\n"
"nOVYdBAYb0ZsahoYjYxim7RNCQ0Sa5z/dFWImcymiPYVhsOrZWA7KmDL9vXztVNl\n"
"nBo5bkMPw7y19w/IVerrPoujm+NxsXU6q3HyV6hB8t5XLfg5GQqEffEhL92gmfJN\n"
"gD5ytMJzzSI2u0Ym5oLF0Tv1fm2vRkG1ni/cWQc3btwqYij5fD/awq8oj1r9S2Sh\n"
"zSnNzdU7cAihGXQKJJRfBxknjdLqUDBL3Gkg0jRNrJW4dCfYh8XqqVSiewd70Hah\n"
"Q1iq+BK2lRV5+HZajc0JMjr4feWmMfWZwoO5bAjNyQKBgQDqGiDe6MMXNjaJKP7f\n"
"f2PSH0FbFmbjXMgnsDuY7xp2DuSmbeTno2eBB3EIBJJzGKrZFRA/AaiFJp6VAt2H\n"
"f8ZR7Qq3ojhrNNk1WsLWNdtz8MLuOlj6ImHi1S5ZZful+/piWJwjPMIc2UI5TI/G\n"
"osPsdxmvNsAlFELUTeTwZrcfCwKBgQDo1+mc0cHZ+g2JMvmw7LzYy2k3Ofxr79+3\n"
"PQSQbvqwhAzhCghg9wdDcSyVCxuHhFRQFPRPxwPHi4xbKrF7xJM06fFt2wxGqp2x\n"
"zSNf2MgFaUhlTMAyfCdxHhtP1OMMYwoK1JTmyH1Ha9C1S769ls0gBAlH0b0da/OO\n"
"rzaKA1YZcwKBgQCO4A97vFeFDSNPpuOXCCqmDUlYDF4TGYdXIand9uAReZl3Mc7o\n"
"yMPc7evAVvoCoOV0w/Q8YAi36xASJcOKdSNW0PMfjDQeygxfEduj3Q7oveJafuXb\n"
"E6zmQfGdwi5J6XWxHB9/Kg/Wh4pxaha9xo0I/s+ALkwq1SjMZhX52tacbQKBgHII\n"
"TADhOW7nZ3mp3V8okdQayoyheV3jPE0hNPsHXv7TrPsJTtEuKgB9jLfxmKnlxOTz\n"
"XjMz5EX1sed0ITfcLbvcVsDWZky1gkVxkcF/CXac+TejPv16kFZ1yCpwuEF4v5JO\n"
"Sh48Uh6ITs57Kg7vq8XiNClYLqYaGE3owW0V9TTBAoGAdI4Zu8IrH3eNvohFdPqW\n"
"4jg8l6QQ7rg9xdyWdK19PfA5fZi1FJ4B7VpadG7uyBFrI1iAlVaJ0/zkbjYVxDw9\n"
"GvplMcNRjEWRvgBsknWtBan/oetyAoDAXvNIAHzkKNqqz3WRaCBvYQ2U4VRfJGV0\n"
"e92dDr7LBv2D2zmS0yJd8W8=\n"
"-----END PRIVATE KEY-----\n";


float read_adc_voltage(void)
{
    adc_select_input(ADC_INPUT);
    uint16_t raw = adc_read();   // valeur entre 0 et 4095
    float voltage = raw * 3.3f / 4095.0f;
    return voltage;
}

static err_t https_sent(void *arg, struct altcp_pcb *conn, u16_t len)
{
    altcp_close(conn);
    return ERR_OK;
}


static err_t https_recv(void *arg, struct altcp_pcb *conn,
                        struct pbuf *p, err_t err)
{

    if (!p) {
        altcp_close(conn);
        return ERR_OK;
    }

    char request[512];
    int len = p->tot_len;
    if (len > 511) len = 511;

    pbuf_copy_partial(p, request, len, 0);
    request[len] = '\0';

    // Route GPIO
    if (strstr(request, "GET /gpio") != NULL) {

    int state = gpio_get(BUTTON_PIN);

    char body[8];
    snprintf(body, sizeof(body), "%d", state);

    char response[256];
    snprintf(response, sizeof(response),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Cache-Control: no-cache\r\n"
        "Content-Length: %d\r\n"
        "Connection: close\r\n"
        "\r\n"
        "%s",
        (int)strlen(body),
        body
    );

    altcp_sent(conn, https_sent);
    altcp_write(conn, response, strlen(response), TCP_WRITE_FLAG_COPY);
    altcp_output(conn);

    pbuf_free(p);
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

    altcp_sent(conn, https_sent);
    altcp_write(conn, response, strlen(response), TCP_WRITE_FLAG_COPY);
    altcp_output(conn);

    pbuf_free(p);
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

    altcp_sent(conn, https_sent);
    altcp_write(conn, response, strlen(response), TCP_WRITE_FLAG_COPY);
    altcp_output(conn);

    pbuf_free(p);
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

    altcp_sent(conn, https_sent);
    altcp_write(conn, response, strlen(response), TCP_WRITE_FLAG_COPY);
    altcp_output(conn);

    pbuf_free(p);
    return ERR_OK;
    }

    // Page principale
    else {

        const char *html1 =
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

            "<input id='input' type='range' min='0' max='3.3' step='0.1' />";

        const char *html2 =
            "<script>"


            "setInterval(async () => {"

            " let r = await fetch('/gpio');"
            " let t = await r.text();"
            " if(t=='1'){document.getElementById('gpio').innerHTML = `<div>La vanne 1 est ouverte</div><div class='vert'></div>`;}"
            " else{document.getElementById('gpio').innerHTML = `<div>La vanne 1 est fermée</div><div class='rouge'></div>`;}"
"setTimeout(() => {"
"  console.log(`Retardée d'une seconde.`);"
"}, '1000');"
            "        let y = await fetch('/voltage');"
            "        let z = await y.text();"
            "        document.getElementById('voltage').innerText = z + ' V';"  

            "}, 2000);"
            
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
            
            "</script>";

        const char *html3 =
            "</body>"
            "</html>";


        altcp_sent(conn, https_sent);
        altcp_write(conn, html1, strlen(html1), TCP_WRITE_FLAG_COPY);
        altcp_output(conn);
        altcp_write(conn, html2, strlen(html2), TCP_WRITE_FLAG_COPY);
        altcp_output(conn);
        altcp_write(conn, html3, strlen(html3), TCP_WRITE_FLAG_COPY);
        altcp_output(conn);

        pbuf_free(p);
        return ERR_OK;
    }
}


static err_t https_accept(void *arg, struct altcp_pcb *conn, err_t err)
{
    altcp_recv(conn, https_recv);
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

    sleep_ms(3000);

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

    printf("IP: %s\n", ip4addr_ntoa(netif_ip4_addr(netif_default)));

    struct altcp_tls_config *tls_config =
        altcp_tls_create_config_server_privkey_cert(
            server_key,
            sizeof(server_key),
            NULL,
            0,
            server_cert,
            sizeof(server_cert)
        );

    if (!tls_config) {
        printf("Erreur TLS config\n");
        while (true) {
            cyw43_arch_poll();
            sleep_ms(1);
        }
    }

    struct altcp_pcb *pcb = altcp_tls_new(tls_config, IPADDR_TYPE_ANY);

    if (!pcb) {
        printf("Erreur PCB\n");
        return 1;
    }

    err_t e = altcp_bind(pcb, IP_ANY_TYPE, 443);

    if (e != ERR_OK) {
        printf("Erreur bind: %d\n", e);
        return 1;
    }

    pcb = altcp_listen(pcb);

    if (!pcb) {
        printf("Erreur listen\n");
        return 1;
    }

    altcp_accept(pcb, https_accept);

    printf("Serveur HTTPS actif\n");


    while (true) {

        // Bouton appuyé = 0 avec pull-up
        if (gpio_get(BUTTON_PIN) == 0) {

            gpio_put(LED_PIN, 1);
//            printf("Vanne ouverte\n");

        } else {

            gpio_put(LED_PIN, 0);
//            printf("Vanne fermée\n");
        }

        cyw43_arch_poll();
        sleep_ms(1);

    }


}