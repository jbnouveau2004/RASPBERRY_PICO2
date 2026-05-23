#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "lwip/altcp.h"
#include "lwip/altcp_tls.h"

#include "server.h"
#include "../routes/routes.h"
#include "../tls/certs.h"

// ***** Serveur HTTP seulement pour pages web (couche abstraite du serveur HTTPS/TLS) *****
static err_t http_accept(void *arg,
                         struct altcp_pcb *conn,
                         err_t err)
{
    altcp_recv(conn, http_recv);

    return ERR_OK;
}

int http_server_start(void)
{
    struct altcp_pcb *http_pcb =
        altcp_new_ip_type(NULL, IPADDR_TYPE_ANY);

    if (!http_pcb) {
        printf("Erreur HTTP PCB\n");
        return -1;
    }

    err_t e = altcp_bind(http_pcb,
                         IP_ANY_TYPE,
                         80);

    if (e != ERR_OK) {
        printf("Erreur bind HTTP: %d\n", e);
        return -1;
    }

    http_pcb = altcp_listen(http_pcb);

    if (!http_pcb) {
        printf("Erreur listen HTTP\n");
        return -1;
    }

    altcp_accept(http_pcb, http_accept);

    printf("Serveur HTTP actif\n");

    return 0;
}

// ***** Serveur HTTPS/TLS pour les API *****
static err_t https_accept(void *arg, struct altcp_pcb *conn, err_t err)
{
    altcp_recv(conn, https_recv);
    return ERR_OK;
}

int https_server_start(void)
{
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
        return -1;
    }

    struct altcp_pcb *pcb = altcp_tls_new(tls_config, IPADDR_TYPE_ANY);

    if (!pcb) {
        printf("Erreur PCB\n");
        return -1;
    }

    err_t e = altcp_bind(pcb, IP_ANY_TYPE, 443);

    if (e != ERR_OK) {
        printf("Erreur bind: %d\n", e);
        return -1;
    }

    pcb = altcp_listen(pcb);

    if (!pcb) {
        printf("Erreur listen\n");
        return -1;
    }

    altcp_accept(pcb, https_accept);

    printf("Serveur HTTPS actif\n");

    return 0;
}