#include <stdio.h>
#include <string.h>

#include "lwip/netif.h"
#include "lwip/ip4_addr.h"

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "lwip/altcp.h"
#include "lwip/altcp_tls.h"
#include "lwip/pbuf.h"



#include "mbedtls/x509_crt.h"
#include "mbedtls/error.h"

#include "mbedtls/pk.h"


#define WIFI_SSID "TP-Link_2B38_EXT"
#define WIFI_PASSWORD "20947170"

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

    const char *body = "<html><body><h1>HTTPS Pico OK</h1></body></html>";

    char response[512];
    snprintf(response, sizeof(response),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: %d\r\n"
        "Connection: close\r\n"
        "\r\n"
        "%s",
        (int)strlen(body),
        body
    );

altcp_sent(conn, https_sent);

err_t wr = altcp_write(conn, response, strlen(response), TCP_WRITE_FLAG_COPY);
if (wr != ERR_OK) {
    printf("ERREUR altcp_write: %d\n", wr);
    pbuf_free(p);
    altcp_close(conn);
    return ERR_OK;
}

altcp_output(conn);

pbuf_free(p);


    return ERR_OK;
}

static err_t https_accept(void *arg, struct altcp_pcb *new_conn, err_t err)
{
    altcp_recv(new_conn, https_recv);
    return ERR_OK;
}

//if (e != ERR_OK) {
//    printf("ERREUR bind: %d\n", e);
//    while (1) sleep_ms(1000);
//}




int main()
{
    stdio_init_all();

sleep_ms(3000);
printf("BOOT OK\n");

    if (cyw43_arch_init()) {
        return 1;
    }

    cyw43_arch_enable_sta_mode();

//    cyw43_arch_wifi_connect_timeout_ms(
//        WIFI_SSID,
//        WIFI_PASSWORD,
//        CYW43_AUTH_WPA2_AES_PSK,
//        30000
//    );

int ret = cyw43_arch_wifi_connect_timeout_ms(
    WIFI_SSID,
    WIFI_PASSWORD,
    CYW43_AUTH_WPA2_AES_PSK,
    30000
);

if (ret) {
    printf("ERREUR WiFi: %d\n", ret);
    while (1) sleep_ms(1000);
}

    printf("Init WiFi...\n");
printf("Connexion WiFi...\n");
printf("IP: %s\n", ip4addr_ntoa(netif_ip4_addr(netif_default)));
printf("Serveur HTTPS lance\n");

//    struct altcp_tls_config *tls_config =
//        altcp_tls_create_config_server_privkey_cert(
//            server_key,
//            sizeof(server_key),
//            NULL,
//            0,
//            server_cert,
//            sizeof(server_cert)
//        );

//struct altcp_tls_config *tls_config =
//    altcp_tls_create_config_server_privkey_cert(
//        server_key,
//        strlen((const char *)server_key) + 1,
//        NULL,
//        0,
//        server_cert,
//        strlen((const char *)server_cert) + 1
//    );

//struct altcp_tls_config *tls_config =
//    altcp_tls_create_config_server_privkey_cert(
//        server_cert,
//        strlen((const char *)server_cert) + 1,
//        server_key,
//        strlen((const char *)server_key) + 1,
//        NULL,
//        0
//    );





mbedtls_x509_crt crt;
mbedtls_x509_crt_init(&crt);

int ret_cert = mbedtls_x509_crt_parse(
    &crt,
    server_cert,
    sizeof(server_cert)
);

if (ret_cert != 0) {
    char errbuf[128];
    mbedtls_strerror(ret_cert, errbuf, sizeof(errbuf));
    printf("CERT REFUSE ret=-0x%04X : %s\n", -ret_cert, errbuf);
} else {
    printf("CERT OK\n");
}

mbedtls_x509_crt_free(&crt);





mbedtls_pk_context pk;
mbedtls_pk_init(&pk);

int ret_key = mbedtls_pk_parse_key(
    &pk,
    server_key,
    sizeof(server_key),
    NULL,
    0,
    NULL,
    NULL
);

if (ret_key != 0) {
    char errbuf[128];
    mbedtls_strerror(ret_key, errbuf, sizeof(errbuf));
    printf("KEY REFUSE ret=-0x%04X : %s\n", -ret_key, errbuf);
} else {
    printf("KEY OK\n");
}

mbedtls_pk_free(&pk);









struct altcp_tls_config *tls_config =
    altcp_tls_create_config_server_privkey_cert(
        server_key,
        sizeof(server_key),
        NULL,
        0,
        server_cert,
        sizeof(server_cert)
    );
            printf("ENTREE create_config_server_privkey_cert\n");


if (!tls_config) {
    printf("ERREUR tls_config NULL\n");
    while (true) {
        cyw43_arch_poll();
        sleep_ms(1);
    }
}


    struct altcp_pcb *pcb = altcp_tls_new(tls_config, IPADDR_TYPE_ANY);
        printf("alloc config...\n");


        if (!pcb) {
    printf("ERREUR pcb NULL\n");
    while (true) {
        cyw43_arch_poll();
        sleep_ms(1);
    }
}



err_t e = altcp_bind(pcb, IP_ANY_TYPE, 443);
    printf("alloc config...\n");

if (e != ERR_OK) {
    printf("ERREUR bind: %d\n", e);
    while (true) {
        cyw43_arch_poll();
        sleep_ms(1);
    }
}


//    altcp_bind(pcb, IP_ANY_TYPE, 443);
    pcb = altcp_listen(pcb);
        printf("alloc config...\n");
    
if (!pcb) {
    printf("ERREUR listen NULL\n");
    while (true) {
        cyw43_arch_poll();
        sleep_ms(1);
    }
}

    altcp_accept(pcb, https_accept);
        printf("alloc config...\n");
    
printf("HTTPS pret sur port 443\n");




    while (true) {
        cyw43_arch_poll();

    static uint32_t last = 0;
    if (to_ms_since_boot(get_absolute_time()) - last > 5000) {
        last = to_ms_since_boot(get_absolute_time());
        printf("IP actuelle: %s\n", ip4addr_ntoa(netif_ip4_addr(netif_default)));
    }


        sleep_ms(1);
    }
}