#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/args_check.h"

/* TODO: get this values from program conf file */
static int  possible_types_count = 1;
static char *possible_types[] = {"IAD731"};

/** Print possible board type names. */
static void possible_types_print(void)
{
    int i;

    for (i = 0; i < possible_types_count; i++)
        printf("%d) %s\n", i + 1, possible_types[i]);
}

/**
 * Print usage for dump_wifi_params.
 *
 * @todo
 *      Print something about manual/wlan/ugw/ folder.
 */
static void usage_print(void)
{
    printf("Usage: ./dump_wifi_params [help|type] "
           "[ipaddr] [pwd]\n");

    printf("help   - print this message\n"
           "type   - board type\n"
           "ipaddr - ip address of boad WAN in v4 format (optional)\n"
           "pwd    - root password (optional)\n");
}

/**
 * Check if board type is correct.
 *
 * @return
 *      Zero on success, or -1, if error occurred.
 */
static int board_type_check(char *board_type)
{
    int i;

    for (i = 0; i < possible_types_count; i++)
    {
        if (!strcmp(board_type, possible_types[i]))
            return 0;
    }

    return -1;
}

/**
 * Check if ipv4 address is correct.
 *
 * @return
 *      Zero on success, or -1, if error occurred.
 */
static int ipv4_check(char *ip)
{
    int i;
    int ip_oktet[IPV4_ALEN];
    char *suffix;

    ip_oktet[0] = strtol(ip, &suffix, 10);
    /* 10 - base */

    if(!ip_oktet_range_check(ip_oktet[0]) || *suffix != '.')
        return -1;

    for(i = 1; i < IPV4_ALEN; i++)
    {
        /* + 1 cause of one dot */
        if(i == IPV4_ALEN - 1)
        {
            ip_oktet[i] = strtol(suffix + 1, &suffix, 10);
            if(!ip_oktet_range_check(ip_oktet[i]) || strlen(suffix) != 0)
                return -1;
        }
        else
        {
            ip_oktet[i] = strtol(suffix + 1, &suffix, 10);
            if(!ip_oktet_range_check(ip_oktet[i]) || *suffix != '.')
                return -1;
        }
    }

    return 0;
}

/**
 * Check if arguments passed to dump_wifi_params are valid.
 *
 * @return
 *      Zero on success, or -1, if error occurred.
 *
 * @se
 *      Prints information about occurred error to stderr.
 *
 * @todo
 *      Add whitespace support in password?
 */
int args_check(int args_count, char **args)
{
    int retval = 0;

    if (args_count == 0 || args_count == 2 || args_count > 3 ||
       (args_count == 1 && !strcmp(args[0], "help")))
    {
        usage_print();
        return -1;
    }

    retval = board_type_check(args[0]);
    if (retval)
    {
        printf("Wrong board type. Possible types are:\n");
        possible_types_print();
        return retval;
    }

    if (args_count == 1)
        return retval;

    retval = ipv4_check(args[1]);
    if (retval)
    {
        printf("Wrong ip format.\n");
        return retval;
    }

    return retval;
}
