#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"

#include "routes.h"
#include "index_html.h"
#include "style_css.h"
#include "script_js.h"

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


// ****** API sur port 443 *****
static err_t https_sent(void *arg, struct altcp_pcb *conn, u16_t len)
{
    altcp_close(conn); // attend la fin des données avant de fermer
    return ERR_OK;
}

err_t https_recv(void *arg, struct altcp_pcb *conn,
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
        "Access-Control-Allow-Origin: http://192.168.1.20\r\n"
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

    // Route toggle vanne 2
    if (strstr(request, "GET /togglevanne2") != NULL) {

        vanne2_state = !vanne2_state;
        gpio_put(vanne2_PIN, vanne2_state);

        const char *body = vanne2_state ? "1" : "0";

        char response[256];
        snprintf(response, sizeof(response),
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Cache-Control: no-cache\r\n"
            "Access-Control-Allow-Origin: http://192.168.1.20\r\n"
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

    // Route voltage
    if (strstr(request, "GET /voltage") != NULL) {
        float voltage = read_adc_voltage();

        char body[64];
        sprintf(body, "%.2f", voltage);

        char response[256];

        sprintf(response,
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: text/plain\r\n"
                "Cache-Control: no-cache\r\n"
                "Access-Control-Allow-Origin: http://192.168.1.20\r\n"
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

    // Route PWM
    if (strstr(request, "GET /pwm") != NULL) {

        float voltage = 0.0f;

        char *v = strstr(request, "v=");
        if (v != NULL) {
            voltage = atof(v + 2);
        }

        // Limitation sécurité
        if (voltage < 0.0f) {
            voltage = 0.0f;
        }

        if (voltage > 3.3f) {
            voltage = 3.3f;
        }

        // Conversion tension -> PWM
        uint slice_num = pwm_gpio_to_slice_num(PWM_PIN);
        uint channel = pwm_gpio_to_channel(PWM_PIN);

        uint16_t level = (uint16_t)(voltage * 4095.0f / 3.3f);

        pwm_set_chan_level(slice_num, channel, level);

        // Corps de réponse
        char body[64];
        snprintf(body, sizeof(body),
            "PWM %.2f V",
            voltage
        );

        // Réponse HTTP complète
        char response[256];
        snprintf(response, sizeof(response),
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Cache-Control: no-cache\r\n"
            "Access-Control-Allow-Origin: http://192.168.1.20\r\n"
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

}


// ***** Page principale sur port 80 *****
static err_t http_sent(void *arg, struct altcp_pcb *conn, u16_t len)
{
    altcp_close(conn); // attend la fin des données avant de fermer
    return ERR_OK;
}

err_t http_recv(void *arg,
                    struct altcp_pcb *conn,
                    struct pbuf *p,
                    err_t err)
{
    if (!p) {
        altcp_close(conn);
        return ERR_OK;
    }

    char request[512];

    int len = p->tot_len;

    if (len > 511)
        len = 511;

    pbuf_copy_partial(p, request, len, 0);

    request[len] = '\0';

    // CSS
    if (strstr(request, "GET /style.css") != NULL) {

        char header[256];

        snprintf(header, sizeof(header),
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/css\r\n"
            "Content-Length: %d\r\n"
            "Connection: close\r\n"
            "\r\n",
            style_css_len
        );

        altcp_sent(conn, http_sent);

        altcp_write(conn, header, strlen(header), TCP_WRITE_FLAG_COPY);

        altcp_write(conn,
                    style_css,
                    style_css_len,
                    TCP_WRITE_FLAG_COPY);

        altcp_output(conn);

        pbuf_free(p);

        return ERR_OK;
    }
    // JS
    if (strstr(request, "GET /script.js") != NULL) {

        char header[256];

        snprintf(header, sizeof(header),
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/javascript\r\n"
            "Cache-Control: no-cache\r\n"
            "Content-Length: %d\r\n"
            "Connection: close\r\n"
            "\r\n",
            script_js_len
        );

        altcp_sent(conn, http_sent);

        altcp_write(conn,
                    header,
                    strlen(header),
                    TCP_WRITE_FLAG_COPY);

        altcp_write(conn,
                    script_js,
                    script_js_len,
                    TCP_WRITE_FLAG_COPY);

        altcp_output(conn);

        pbuf_free(p);

        return ERR_OK;
    }
    // HTML
    if (strstr(request, "GET / ") != NULL ||
        strstr(request, "GET /HTTP") != NULL ||
        strstr(request, "GET / HTTP") != NULL) {

        char header[256];

        snprintf(header, sizeof(header),
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "Cache-Control: no-cache\r\n"
            "Content-Length: %d\r\n"
            "Connection: close\r\n"
            "\r\n",
            index_html_len
        );

        altcp_sent(conn, http_sent);

        altcp_write(conn,
                    header,
                    strlen(header),
                    TCP_WRITE_FLAG_COPY);

        altcp_write(conn,
                    index_html,
                    index_html_len,
                    TCP_WRITE_FLAG_COPY);

        altcp_output(conn);

        pbuf_free(p);

        return ERR_OK;
    }

}