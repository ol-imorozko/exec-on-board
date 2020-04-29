#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>

#include "telnet_remote_control.h"

#define STANDARD_IP_ADDR    "192.168.1.1"
#define STANDARD_USERNAME   "admin"
#define STANDARD_PASSWORD   "admin"
#define TELNET_PORT         23
#define RECV_BUFF_SIZE      10000
#define CMD_BUFF_SIZE       1000
#define TIMEOUT             3

/** create AF_INET, SOCK_STREAM socket */
static int socket_create(void)
{
    int fd;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    return fd;
}

/**
 * Fill 'ret' structure with appropriate data.
 * If 'ip_addr', is not given (equal to NULL),
 * function will use standard defined value.
 *
 * @return
 *      Zero on success, or -1, if error occured.
 *
 * @se
 *      Prints information about occurred error to stderr.
 *
 * @sa
 *      telnet_fill_auth_data
 */
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

    ret->dst_addr = strdup(ip_addr ? ip_addr : STANDARD_IP_ADDR);

    if (!ret->dst_addr)
    {
        perror("Could not duplicate string for dst_addr\n");
        return -1;
    }

    return 0;
}

/** Free allocated memory and close socket for 'data'. */
void telnet_free_auth_data(telnet_auth_data *data)
{
    free(data->tcp_conn.dst_addr);
    free(data->recv_buff);
    free(data->username);
    free(data->password);
    free(data->exec_cmd);
    close(data->tcp_conn.fd);
}

/**
 * Fill 'ret' structure with appropriate data.
 * If 'ip_addr', 'password', or 'username' is not given (equal to NULL),
 * function will use standard defined values.
 *
 * @return
 *      Zero on success, or -1, if error occured.
 *
 * @se
 *      Prints information about occurred error to stderr.
 */
int telnet_fill_auth_data(telnet_auth_data *ret, char *ip_addr,
                          char *username, char *password)
{
    int retval;

    ret->recv_buff  = NULL;
    ret->username   = NULL;
    ret->password   = NULL;
    ret->exec_cmd   = NULL;

    retval = tcp_fill_conn_info(&ret->tcp_conn, ip_addr);
    if (retval)
        goto cleanup;

    ret->recv_buff = calloc(RECV_BUFF_SIZE, sizeof(char));
    if (!ret->recv_buff)
    {
        perror("Could not allocate memory for receive buffer\n");
        goto cleanup;
    }

    ret->exec_cmd = calloc(CMD_BUFF_SIZE, sizeof(char));
    if (!ret->recv_buff)
    {
        perror("Could not allocate memory for execution command\n");
        goto cleanup;
    }

    ret->username = strdup(username ? username : STANDARD_USERNAME"\r");
    if (!ret->username)
    {
        perror("Could not duplicate string for username\n");
        goto cleanup;
    }

    ret->password = strdup(password ? password : STANDARD_PASSWORD"\r");
    if (!ret->password)
    {
        perror("Could not duplicate string for password\n");
        goto cleanup;
    }

    return retval;

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

    return retval;
}

/**
 * Wait for TIMEOUT seconds until server sends data that contains
 * 'expected' as substring.
 *
 * @return
 *      Zero on success, or -1, if TIMEOUT reached or error occured.
 *
 * @se
 *      Prints information about occurred error to stderr.
 */
static int telnet_recv_str(telnet_auth_data *data, char *expected)
{
    int retval;
    int offset;
    struct timeval tv;
    tv.tv_sec   = TIMEOUT;
    tv.tv_usec  = 0;
    offset      = 0;

    retval = setsockopt(data->tcp_conn.fd, SOL_SOCKET, SO_RCVTIMEO,
                       (char *)&tv, sizeof(tv));
    if (retval)
    {
        perror("Could not set socket option\n");
        return retval;
    }

    while (1)
    {
        retval = recv(data->tcp_conn.fd, data->recv_buff + offset,
                      RECV_BUFF_SIZE, 0);
        if (retval == -1)
        {
            perror("Could not receive data from server\n");
            break;
        }

        offset += retval;

        if (strstr(data->recv_buff, expected) != NULL)
        {
            memset(data->recv_buff, 0, offset);
            return 0;
        }
    }

    memset(data->recv_buff, 0, retval);

    return retval;
}

/**
 * Send string 'str' to remote telnet server.
 *
 * @return
 *      Zero on success, or -1, if error occurred.
 *
 * @se
 *      Prints information about occurred error to stderr.
 */
static int telnet_send_str(telnet_auth_data *data, char *str)
{
    int retval;

    retval = send(data->tcp_conn.fd, str, strlen(str), 0);
    if (retval == -1)
    {
        perror("Could not send data to server\n");
        return retval;
    }

    return 0;
}

/**
 * Authorise on remote telnet server
 * by username and password specified in 'data'.
 * Different telnet servers could have different responces
 * for users, so we must specify string we are waiting for.
 *
 * @param   expected_login_responce
 *      The string we are waiting from the server,
 *      so we can enter username.
 *
 * @param   expected_password_responce
 *      The string we are waiting from the server,
 *      so we can enter password.
 *
 * @param   expected_auth_responce
 *      The string we are waiting from the server
 *      after successful authentication.
 *
 * @return
 *      Zero on success, or -1, if error occurred.
 *
 * @se
 *      Prints information about occurred error to stderr.
 *
 */
int telnet_auth(telnet_auth_data *data,
                char *expected_login_responce,
                char *expected_password_responce,
                char *expected_auth_responce)
{
    int retval;

    retval = tcp_connection_establish(&data->tcp_conn);
    if (retval)
        return retval;

    retval = telnet_recv_str(data, expected_login_responce);
    if (retval)
    {
        fprintf(stderr, "Could not reach expected responce\n");
        return retval;
    }

    retval = telnet_send_str(data, data->username);
    if (retval)
        return retval;

    retval = telnet_recv_str(data, expected_password_responce);
    if (retval)
    {
        fprintf(stderr, "Could not reach expected responce\n");
        return retval;
    }

    retval = telnet_send_str(data, data->password);
    if (retval)
        return retval;

    retval = telnet_recv_str(data, expected_auth_responce);
    if (retval)
    {
        fprintf(stderr, "Maybe login or password is incorrect\n");
        return retval;
    }

    return retval;
}

/**
 * Execute command specified in 'data' on remote telnet server.
 *
 * @param   expected_responce
 *      The string we are waiting from the server
 *      after successful execution.
 *
 * @return
 *      Zero on success, or -1, if error occurred.
 *
 * @se
 *      Prints information about occurred error to stderr.
 *
 */
int telnet_execute_command(telnet_auth_data *data,
                           char *expected_responce)
{
    int retval;

    retval = telnet_send_str(data, data->exec_cmd);
    if (retval)
        return retval;

    retval = telnet_recv_str(data, expected_responce);
    if (retval)
    {
        fprintf(stderr, "Could not reach expected responce\n");
        return retval;
    }

    return retval;
}
