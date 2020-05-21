/** @file
 * @brief Implementation of simple TFTP server.
 *
 * The server listens on a server specified port.
 * It can handle read requests and serve files via TFTP protocol
 * as defined in RFC 1350.
 * It only serves files from a server specified directory.
 *
 * @author Ivan Morozko <Ivan.Morozko@oktetlabs.ru>
 *
 * $Id: $
 */

#ifndef _TFTP_SERVER_
#define _TFTP_SERVER_

#include "connection.h"

typedef struct tftp_server_data {
    conn_info   udp_conn;
    char        *base_directory;
} tftp_server_data;

extern int tftp_fill_server_data(tftp_server_data *ret, char *ip_addr,
                                 int port, char *base_directory);

extern int tftp_server_start(tftp_server_data *srv_data);

static inline void tftp_server_stop(int pid)
{
    kill(pid, SIGTERM);
}

#endif
