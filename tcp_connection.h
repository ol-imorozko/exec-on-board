#ifndef _TCP_CONNECTION_
#define _TCP_CONNECTION_

#include <stdbool.h>
#include <stdint.h>

typedef struct tcp_conn_info {
    int         fd;
    int         dst_port;
    char        *dst_addr;
} tcp_conn_info;

extern int tcp_connection_establish(tcp_conn_info *info);

#endif
