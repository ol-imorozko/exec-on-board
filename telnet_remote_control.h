#ifndef _TELNET_REMOTE_CONTROL_
#define _TELNET_REMOTE_CONTROL_

#include "tcp_connection.h"

#define STANDARD_IP_ADDR    "192.168.1.1"
#define TELNET_PORT         23
#define RECV_BUFF_SIZE      1000
#define CMD_BUFF_SIZE       1000
#define TIMEOUT             3

typedef struct telnet_auth_data {
    tcp_conn_info   tcp_conn;
    char            *recv_buff;
    char            *username;
    char            *password;
    char            *exec_cmd;
} telnet_auth_data;

/**
 *
 *
 * @todo
 *      Add board type cause username can depend on it.
 */
extern int telnet_fill_auth_data(telnet_auth_data *ret, char *ip_addr);

extern void telnet_free_auth_data(telnet_auth_data *data);

extern int telnet_recv_specific_data(telnet_auth_data *fata, char *expected);

extern int telnet_auth(telnet_auth_data *data);

#endif
