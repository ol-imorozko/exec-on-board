/** @file
 * @brief Arguments checking for exec_on_board tool.
 *
 * @author Ivan Morozko <Ivan.Morozko@oktetlabs.ru>
 *
 * $Id: $
 */

#ifndef _ARGS_CHECK
#define _ARGS_CHECK

#include "include/telnet_remote_control.h"
#include "include/tftp_server.h"

#define FLAG_QUIET               1

typedef struct exec_on_board_options {
    int                     flags;
    telnet_auth_options     telnet_opt;
    tftp_server_options     tftp_opt;
} exec_on_board_options;

extern int options_get(int argc, char **argv);

#endif
