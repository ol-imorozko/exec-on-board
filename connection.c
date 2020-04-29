#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "connection.h"

/**
 * Perform connect on socket.
 *
 * @return
 *      Zero on success, or -1, if error occurred.
 *
 * @se
 *      Prints information about occurred error to stderr.
 */
int socket_connect(conn_info *info)
{
    int retval;
    int remote_size;
    struct sockaddr_in remote;

    remote.sin_addr.s_addr  = inet_addr(info->addr);
    remote.sin_family       = AF_INET;
    remote.sin_port         = htons(info->port);
    remote_size             = sizeof(struct sockaddr_in);

    retval = connect(info->sock, (struct sockaddr *)&remote, remote_size);
    if (retval)
    {
        perror("Connection refused by remote host\n");
        return retval;
    }

    return 0;
}

/**
 * Perform bind on socket.
 *
 * @return
 *      Zero on success, or -1, if error occurred.
 *
 * @se
 *      Prints information about occurred error to stderr.
 */
int socket_bind(conn_info *info)
{
    int retval;
    int remote_size;
    struct sockaddr_in remote;

    remote.sin_addr.s_addr  = inet_addr(info->addr);
    remote.sin_family       = AF_INET;
    remote.sin_port         = htons(info->port);
    remote_size             = sizeof(struct sockaddr_in);

    retval = bind(info->sock, (struct sockaddr *)&remote, remote_size);
    if (retval)
    {
        perror("Could not bind to remote host\n");
        return retval;
    }

    return 0;
}