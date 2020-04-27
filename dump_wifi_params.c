#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

#include "args_check.h"
#include "telnet_remote_control.h"

//error while filling.... print errors to stdout.
//all other erors should be printed to stderr!
int main(int argc, char **argv)
{
    int retval = 0;
    telnet_auth_data board_control_data;

    /* We don't want argv[0] which represents name of the program */
    retval = args_check(argc - 1, argv + 1);
    if (retval)
        return retval;

    retval = telnet_fill_auth_data(&board_control_data, NULL);
    if (retval)
        return retval;

    retval = telnet_auth(&board_control_data);
    if (retval)
        return retval;

    return retval;
}

