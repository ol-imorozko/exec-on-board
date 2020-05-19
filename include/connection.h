/** @file
 * @brief Wrapper for bind and connect functions.
 *
 * @author Ivan Morozko <Ivan.Morozko@oktetlabs.ru>
 *
 * $Id: $
 */

#ifndef _CONNECTION_
#define _CONNECTION_

#include <stdbool.h>
#include <stdint.h>

typedef struct conn_info {
    int         sock;
    int         port;
    char        *addr;
} conn_info;

extern int socket_bind(conn_info *info);

extern int socket_connect(conn_info *info);

extern int conn_info_fill(conn_info *ret, char *ip_addr,
                          int port, int sock_type);

#endif
