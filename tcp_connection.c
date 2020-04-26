#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "tcp_connection.h"

/** Try to connect with server. */
static int tcp_socket_connect(tcp_conn_info *info)
{
    int retval;
    int remote_size;
    struct sockaddr_in remote;

    remote.sin_addr.s_addr  = inet_addr(info->dst_addr);
    remote.sin_family       = AF_INET;
    remote.sin_port         = htons(info->dst_port);
    remote_size             = sizeof(struct sockaddr_in);

    retval = connect(info->fd, (struct sockaddr *)&remote, remote_size);
    return retval;
}

/**
 * Establish tcp connection.
 *
 * @return
 *      Zero on success, or -1, if error occurred.
 *
 * @se
 *      Prints information about occurred error to stderr.
 */
int tcp_connection_establish(tcp_conn_info *info)
{
    int retval;

    retval = tcp_socket_connect(info);
    if (retval)
    {
        perror("Connection refused by remote host\n");
        return retval;
    }

    return 0;
}
