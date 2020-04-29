/** @file
 * @brief Arguments checking for dump_wifi_params tool.
 *
 * @author Ivan Morozko <Ivan.Morozko@oktetlabs.ru>
 *
 * $Id: $
 */

#ifndef _ARGS_CHECK
#define _ARGS_CHECK

#define IPV4_ALEN 4

#define ip_oktet_range_check(ip_oktet) \
    ((ip_oktet >= 0) && (ip_oktet <= 255))

extern int args_check(int argc, char **argv);

#endif
