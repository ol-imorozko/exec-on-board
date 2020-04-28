#ifndef _TELNET_REMOTE_CONTROL_
#define _TELNET_REMOTE_CONTROL_

#include "tcp_connection.h"

typedef struct telnet_auth_data {
    tcp_conn_info   tcp_conn;
    char            *recv_buff;
    char            *username;
    char            *password;
    char            *exec_cmd;
} telnet_auth_data;

typedef enum send_str_variants {
    USERNAME,
    PASSWORD,
    COMMAND
} send_str_variants;

extern int telnet_fill_auth_data(telnet_auth_data *ret, char *ip_addr,
                                 char *username, char *password);

extern void telnet_free_auth_data(telnet_auth_data *data);

extern int telnet_auth(telnet_auth_data *data,
                       char *expected_login_responce,
                       char *expected_password_responce);

extern int telnet_execute_command(telnet_auth_data *data,
                                  char *expected_responce);
#endif
