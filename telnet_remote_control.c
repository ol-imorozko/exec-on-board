#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>

#include "telnet_remote_control.h"

#define TELNET_PORT         23
#define MAX_RECV_BUFF_SIZE  10000
#define MAX_CMD_BUFF_SIZE   1000
#define TIMEOUT             3

/**
 * Fill 'ret' structure with appropriate data.
 *
 * @return
 *      Zero on success, or -1, if error occured.
 *
 */
int telnet_fill_auth_data(telnet_auth_data *ret, char *ip_addr,
                          char *username, char *password)
{
    int retval;

    retval = conn_info_fill(&ret->tcp_conn, ip_addr,
                            TELNET_PORT, SOCK_STREAM);
    if (retval)
        return retval;

    ret->username = username;
    ret->password = password;

    return retval;
}

/**
 * Close socket for telnet_auth_data
 *
 * @return
 *      Zero on success, or -1, if error occured.
 *
 * @se
 *      Prints information about occurred error to stderr.
 *
 */
int telnet_free_auth_data(telnet_auth_data *data)
{
    int retval;

    retval = close(data->tcp_conn.sock);
    if (retval)
        perror("Could not close socket\n");

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
static int telnet_recv_str(telnet_auth_data *data, char *expected, char *buff)
{
    int retval;
    int offset;
    struct timeval tv;
    tv.tv_sec   = TIMEOUT;
    tv.tv_usec  = 0;
    offset      = 0;

    retval = setsockopt(data->tcp_conn.sock, SOL_SOCKET, SO_RCVTIMEO,
                       (char *)&tv, sizeof(tv));
    if (retval)
    {
        perror("Could not set socket option\n");
        return retval;
    }

    while (1)
    {
        retval = recv(data->tcp_conn.sock, buff + offset,
                      MAX_RECV_BUFF_SIZE, 0);
        if (retval == -1)
        {
            perror("Could not receive data from server\n");
            break;
        }

        offset += retval;

        if (strstr(buff, expected) != NULL)
        {
            memset(buff, 0, offset);
            return 0;
        }
    }

    memset(buff, 0, retval);

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

    retval = send(data->tcp_conn.sock, str, strlen(str), 0);
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
    char recv_buff[MAX_RECV_BUFF_SIZE] = {0};

    retval = socket_connect(&data->tcp_conn);
    if (retval)
        return retval;

    retval = telnet_recv_str(data, expected_login_responce, recv_buff);
    if (retval)
    {
        fprintf(stderr, "Could not reach expected responce\n");
        return retval;
    }

    retval = telnet_send_str(data, data->username);
    if (retval)
        return retval;

    retval = telnet_recv_str(data, expected_password_responce, recv_buff);
    if (retval)
    {
        fprintf(stderr, "Could not reach expected responce\n");
        return retval;
    }

    retval = telnet_send_str(data, data->password);
    if (retval)
        return retval;

    retval = telnet_recv_str(data, expected_auth_responce, recv_buff);
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
                           char *command,
                           char *expected_responce)
{
    int retval;
    char command_buff[MAX_CMD_BUFF_SIZE] = {0};

    retval = telnet_send_str(data, command);
    if (retval)
        return retval;

    retval = telnet_recv_str(data, expected_responce, command_buff);
    if (retval)
    {
        fprintf(stderr, "Could not reach expected responce\n");
        return retval;
    }

    return retval;
}
