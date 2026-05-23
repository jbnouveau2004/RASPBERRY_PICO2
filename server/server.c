#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "lwip/altcp.h"
#include "lwip/altcp_tls.h"

#include "server.h"
#include "../routes/routes.h"
#include "../tls/certs.h"

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