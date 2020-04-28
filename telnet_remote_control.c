#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>

#include "telnet_remote_control.h"

#define STANDARD_IP_ADDR    "192.168.1.1"
#define STANDARD_USERNAME   "admin"
#define STANDARD_PASSWORD   "1admin"
#define TELNET_PORT         23
#define RECV_BUFF_SIZE      1000
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

/** Free allocated memory for 'data'. */
void telnet_free_auth_data(telnet_auth_data *data)
{
    free(data->tcp_conn.dst_addr);
    free(data->recv_buff);
    free(data->username);
    free(data->password);
    free(data->exec_cmd);
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
static int telnet_recv_specific_str(telnet_auth_data *data, char *expected)
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
            perror("Could not set socket option\n");
            break;
        }

        retval = recv(data->tcp_conn.fd, data->recv_buff, RECV_BUFF_SIZE, 0);
        if (retval == -1)
        {
            perror("Could not receive data from server\n");
            break;
        }

        printf("received data:\n%s\n expected string:%s\n", data->recv_buff, expected);
        if(strstr(data->recv_buff, expected) != NULL)
            return 0;
    }

    return retval;
}

/**
 * Send string specified by 'v' to remote telnet server.
 *
 * @param   v       Enum: USERNAME, PASSWORD, COMMAND.
 *
 * @return
 *      Zero on success, or -1, if error occurred.
 *
 * @se
 *      Prints information about occurred error to stderr.
 */
static int telnet_send_str(telnet_auth_data *data, send_str_variants v)
{
    int retval;
    char *send_str;

    switch (v)
    {
        case USERNAME:
            send_str = data->username;
            break;

        case PASSWORD:
            send_str = data->password;
            break;

        case COMMAND:
            send_str = data->exec_cmd;
            break;
    }

    retval = send(data->tcp_conn.fd, send_str, sizeof(send_str), 0);
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
 * @return
 *      Zero on success, or -1, if error occurred.
 *
 */
int telnet_auth(telnet_auth_data *data,
                char *expected_login_responce,
                char *expected_password_responce)
{
    int retval;

    printf("step 1\n");
    retval = tcp_connection_establish(&data->tcp_conn);
    if(retval)
        return retval;

    printf("step 2\n");
    retval = telnet_recv_specific_str(data, expected_login_responce);
    if(retval)
        return retval;

    printf("step 3\n");
    retval = telnet_send_str(data, USERNAME);
    if(retval)
        return retval;

    printf("step 4\n");
    retval = telnet_recv_specific_str(data, expected_password_responce);
    if(retval)
        return retval;

    printf("step 5\n");
    retval = telnet_send_str(data, PASSWORD);
    if(retval)
        return retval;

    printf("step 6\n");
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
 */
int telnet_execute_command(telnet_auth_data *data,
                           char *expected_responce)
{
    int retval;

    printf("Command to execute: %s", data->exec_cmd);

    retval = telnet_send_str(data, COMMAND);
    if(retval)
        return retval;

    retval = telnet_recv_specific_str(data, expected_responce);
    if(retval)
        return retval;

    return retval;
}
