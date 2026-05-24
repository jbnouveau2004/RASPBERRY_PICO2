// lwipopts.h

#ifndef LWIPOPTS_H
#define LWIPOPTS_H

#define NO_SYS                      1
#define LWIP_SOCKET                 0
#define LWIP_NETCONN                0

#define MEM_LIBC_MALLOC             0
#define MEM_ALIGNMENT               4
#define MEM_SIZE                    4000
#define MEMP_NUM_TCP_PCB            5
#define MEMP_NUM_TCP_PCB_LISTEN     2
#define MEMP_NUM_TCP_SEG            16
#define MEMP_NUM_SYS_TIMEOUT        10

#define PBUF_POOL_SIZE              24
#define PBUF_POOL_BUFSIZE           1520

#define LWIP_TCP                    1
#define TCP_TTL                     255
#define TCP_WND                     4096
#define TCP_SND_BUF                 4096
#define TCP_MSS                     1460

#define LWIP_ICMP                   1
#define LWIP_DHCP                   1
#define LWIP_DNS                    1
#define LWIP_IPV4                   1
#define LWIP_IPV6                   0

#define LWIP_RAW                    1
#define LWIP_UDP                    1

#define LWIP_STATS                  0
#define LWIP_PROVIDE_ERRNO          1

#define LWIP_NETIF_HOSTNAME         1




#define ALTCP_MBEDTLS_USE_SESSION_CACHE 0

#define LWIP_ALTCP 1
#define LWIP_ALTCP_TLS 1
#define LWIP_ALTCP_TLS_MBEDTLS 1




// augmentation de la mémoire (est-ce necessaire?)
/**
#define MEM_SIZE (48 * 1024)
#define MEMP_NUM_TCP_PCB 6
#define MEMP_NUM_TCP_SEG 16
#define TCP_SND_BUF (4 * TCP_MSS)
#define TCP_WND (4 * TCP_MSS)
*/


#define MEM_SIZE (192 * 1024)

#define MEMP_NUM_TCP_PCB 10
#define MEMP_NUM_TCP_SEG 64

#define TCP_SND_BUF (16 * TCP_MSS)
#define TCP_WND     (8 * TCP_MSS)


/**

MEM_SIZE : mémoire globale lwIP. Si trop petit, reset/saturation.

MEMP_NUM_TCP_PCB : nombre de connexions TCP possibles. Avec HTTP + HTTPS + Chrome, 6 peut être juste.

MEMP_NUM_TCP_SEG : nombre de segments TCP en attente. 16 peut être trop bas pour fichiers JS/CSS + fetch.

TCP_SND_BUF : buffer d’envoi. Très important pour envoyer script.js.

TCP_WND : fenêtre de réception. Moins critique que TCP_SND_BUF, mais tu peux l’augmenter aussi.

*/


#endif