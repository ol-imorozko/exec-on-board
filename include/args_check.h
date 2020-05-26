/** @file
 * @brief Arguments checking for dump_wifi_params tool.
 *
 * @author Ivan Morozko <Ivan.Morozko@oktetlabs.ru>
 *
 * $Id: $
 */

#ifndef _ARGS_CHECK
#define _ARGS_CHECK

#include "include/telnet_remote_control.h"
#include "include/tftp_server.h"

#define QUIET       1
#define USE_TFTP    2

/* Default options. */
#define STD_FLAGS                0
#define STD_BOARD_ADDR           "192.168.1.1"
#define STD_TELNET_PORT          "23"
#define STD_USERNAME             "admin"
#define STD_PASSWORD             "admin"
#define STD_LOGIN_RESPONCE       "login:"
#define STD_PASSWORD_RESPONCE    "Password:"
#define STD_CL_PROMPT            "root@ugwcpe:~#"
#define STD_TFTP_ADDR            "192.168.1.3"
#define STD_TFTP_PORT            "12345"
#define STD_TFTP_BASE_DIRECTORY  "."

typedef struct dump_params_options {
    int                     flags;
    telnet_auth_options     telnet_opt;
    tftp_server_options     tftp_opt;
} dump_params_options;

/* Set defaults */
static dump_params_options global_opt = {
    .flags                           = STD_FLAGS,
    .telnet_opt.addr                 = STD_BOARD_ADDR,
    .telnet_opt.port                 = STD_TELNET_PORT,
    .telnet_opt.username             = STD_USERNAME,
    .telnet_opt.password             = STD_PASSWORD,
    .telnet_opt.login_responce       = STD_LOGIN_RESPONCE,
    .telnet_opt.password_responce    = STD_PASSWORD_RESPONCE,
    .telnet_opt.cl_prompt            = STD_CL_PROMPT,
    .tftp_opt.addr                   = STD_TFTP_ADDR,
    .tftp_opt.port                   = STD_TFTP_PORT,
    .tftp_opt.base_directory         = STD_TFTP_BASE_DIRECTORY,
};

extern int args_check(int argc, char **argv);

#endif
