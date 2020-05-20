/** @file
 * @brief Interface for executing commands via telnet.
 *
 * Functions for authorising and executing commands on
 * remote telnet server.
 *
 * @author Ivan Morozko <Ivan.Morozko@oktetlabs.ru>
 *
 * $Id: $
 */

#ifndef _TELNET_REMOTE_CONTROL_
#define _TELNET_REMOTE_CONTROL_

#include "connection.h"

typedef struct telnet_auth_data {
    conn_info       tcp_conn;
    char            *recv_buff;
    char            *username;
    char            *password;
    char            *exec_cmd;
} telnet_auth_data;

extern int telnet_fill_auth_data(telnet_auth_data *ret, char *ip_addr,
                                 char *username, char *password);

extern int telnet_free_auth_data(telnet_auth_data *data);

extern int telnet_auth(telnet_auth_data *data,
                       char *expected_login_responce,
                       char *expected_password_responce,
                       char *expected_auth_responce);

extern int telnet_execute_command(telnet_auth_data *data,
                                  char *command, char *expected_responce,
                                  char *error_substr);
#endif
