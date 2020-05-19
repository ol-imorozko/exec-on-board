#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

#include "include/args_check.h"
#include "include/telnet_remote_control.h"

/* TODO: error while filling.... print errors to stdout.
all other erors should be printed to stderr! */
int main(int argc, char **argv)
{
    int retval = 0;
    telnet_auth_data board_control_data;

    /* We don't want argv[0] which represents name of the program */
    retval = args_check(argc - 1, argv + 1);
    if (retval)
        return retval;

    retval = telnet_fill_auth_data(&board_control_data, "192.168.1.1", "admin\r", "admin\r");
    if (retval)
        return retval;

    retval = telnet_auth(&board_control_data, "login:", "Password:", "root@ugwcpe:~#");
    if (retval)
        goto cleanup;


    return retval;

cleanup:
    telnet_free_auth_data(&board_control_data);
    return retval;
}

