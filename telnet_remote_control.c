#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>

#include "telnet_remote_control.h"

typedef enum auth_info_variants
{
    USERNAME,
    PASSWORD
} auth_info_variants;

static int socket_create(void)
{
    int fd;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    return fd;
}

static int tcp_fill_conn_info(tcp_conn_info *ret, char *ip_addr)
{
    ret->dst_addr = NULL;

    ret->fd = socket_create();
    if (ret->fd == -1)
    {
        perror("Could not create socket\n");
        return -1;
    }

    ret->dst_port = TELNET_PORT;

    ret->dst_addr = ip_addr ? strdup(ip_addr) : strdup(STANDARD_IP_ADDR);
    if (!ret->dst_addr)
    {
        perror("Could not duplicate string dst_addr\n");
        return -1;
    }

    return 0;
}

void telnet_free_auth_data(telnet_auth_data *data)
{
    free(data->tcp_conn.dst_addr);
    free(data->recv_buff);
    free(data->username);
    free(data->password);
    free(data->exec_cmd);
}

int telnet_fill_auth_data(telnet_auth_data *ret, char *ip_addr)
{
    int retval;

    ret->recv_buff  = NULL;
    ret->username   = NULL;
    ret->password   = NULL;
    ret->exec_cmd   = NULL;

    retval = tcp_fill_conn_info(&ret->tcp_conn, ip_addr);
    if (retval)
        goto cleanup;

    ret->recv_buff = malloc(RECV_BUFF_SIZE);
    if (!ret->recv_buff)
    {
        perror("Could not allocate memory for receive buffer\n");
        goto cleanup;
    }

    ret->exec_cmd = malloc(CMD_BUFF_SIZE);
    if (!ret->recv_buff)
    {
        perror("Could not allocate memory for execution command\n");
        goto cleanup;
    }

    ret->username = strdup("admin\r");
    if (!ret->username)
    {
        perror("Could not duplicate string username\n");
        goto cleanup;
    }

    ret->password = strdup("admin\r");
    if (!ret->password)
    {
        perror("Could not duplicate string password\n");
        goto cleanup;
    }

    return 0;

cleanup:
    if (ret->tcp_conn.dst_addr)
        free(ret->tcp_conn.dst_addr);

    if (ret->recv_buff)
        free(ret->recv_buff);

    if (ret->password)
        free(ret->password);

    if (ret->username)
        free(ret->username);

    if (ret->exec_cmd)
        free(ret->exec_cmd);

    return -1;
}

/**
 * Wait for TIMEOUT seconds until server sends data that contains
 * @expected as substring.
 *
 * @return
 *      Zero, if server reply contains @expected, or -1, if TIMEOUT reached
 *      or error occured.
 */
int telnet_recv_specific_data(telnet_auth_data *data, char *expected)
{
    int retval;
    struct timeval tv;
    tv.tv_sec = TIMEOUT;
    tv.tv_usec = 0;

    while (1)
    {
        retval = setsockopt(data->tcp_conn.fd, SOL_SOCKET, SO_RCVTIMEO,
                            (char *)&tv, sizeof(tv));
        if (retval)
        {
            perror("Could not setsockopt\n");
            break;
        }

        retval = recv(data->tcp_conn.fd, data->recv_buff, RECV_BUFF_SIZE, 0);
        if (retval == -1)
        {
            perror("Could not receive data from server\n");
            break;
        }

        if(strstr(data->recv_buff, expected) != NULL)
            return 0;
    }

    return retval;
}

static int telnet_send_auth_info(telnet_auth_data *data, auth_info_variants v)
{
    int retval;
    char *send_data;

    send_data = (v == USERNAME) ? data->username : data->password;

    retval = send(data->tcp_conn.fd, send_data, sizeof(send_data), 0);
    if (retval == -1)
    {
        perror("Could not send data to server\n");
        return retval;
    }

    return 0;
}

int telnet_auth(telnet_auth_data *data)
{
    int retval;

    retval = tcp_connection_establish(&data->tcp_conn);
    if(retval)
        return retval;

    retval = telnet_recv_specific_data(data, "login:");
    if(retval)
        return retval;

    retval = telnet_send_auth_info(data, USERNAME);
    if(retval)
        return retval;

    retval = telnet_recv_specific_data(data, "Password:");
    if(retval)
        return retval;

    retval = telnet_send_auth_info(data, PASSWORD);
    if(retval)
        return retval;

    return retval;
}





















