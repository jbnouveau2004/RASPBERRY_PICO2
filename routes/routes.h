#ifndef ROUTES_H
#define ROUTES_H

#include "lwip/altcp.h"
#include "lwip/pbuf.h"

err_t https_recv(void *arg,
                 struct altcp_pcb *conn,
                 struct pbuf *p,
                 err_t err);

#endif