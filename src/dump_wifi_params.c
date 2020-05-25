#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <signal.h>
#include <sys/wait.h>

#include <unistd.h>

#include "include/args_check.h"
#include "include/telnet_remote_control.h"
#include "include/tftp_server.h"

static telnet_auth_options tmp_telnet_opts = {
    .addr               = "192.168.1.1",
    .port               = "23",
    .username           = "admin",
    .password           = "admin",
    .login_responce     = "login:",
    .password_responce  = "Password:",
    .cl_prompt          = "root@ugwcpe:~#",
};

static telnet_cmd_data tmp_get_backup_cmd = {
    .command            = "tftp -g -l sample_file 192.168.1.3 12345",
    .error_substr       = "tftp:",
};

int main(int argc, char **argv)
{
    int                 retval;
    int                 tftp_server_pid;
    int                 tftp_server_status;
    telnet_board_data   board_control_data;
    tftp_server_data    srv_data;

    /* Don't want argv[0] which represents name of the program */
    retval = args_check(argc - 1, argv + 1);
    if (retval)
        return retval;

    retval = tftp_fill_server_data(&srv_data, "192.168.1.3", 12345, ".");
    if (retval)
        return retval;

    retval = telnet_fill_board_data(&board_control_data, &tmp_telnet_opts);
    if (retval)
        return retval;

    tftp_server_pid = fork();

    switch(tftp_server_pid)
    {
        case -1:
            perror("dump_wifi_params: fork()");
            goto cleanup;
        case 0:
            retval = tftp_server_start(&srv_data);
            exit(retval ? EXIT_FAILURE : EXIT_SUCCESS);
        default:
            retval = telnet_auth(&board_control_data);
            if (retval)
                goto cleanup;

            retval = telnet_execute_command(&board_control_data,
                                            &tmp_get_backup_cmd);
            if (retval)
                goto cleanup;

            telnet_free_board_data(&board_control_data);
            tftp_server_stop(tftp_server_pid);
            wait(&tftp_server_status);
            return retval;
    }

cleanup:
    telnet_free_board_data(&board_control_data);
    tftp_server_stop(tftp_server_pid);
    wait(&tftp_server_status);
    return retval;
}
