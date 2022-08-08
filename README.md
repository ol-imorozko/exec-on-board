# exec-on-board
Single-binary easy-to-use tool for downloading files via Telnet + TFTP written in pure C.

Usually, embedded devices have Telnet and TFTP servers installed for command line access and file downloading/uploading, respectively.
You can use this tool to ease the manual task of downloading some files from your target device or executing arbitrary commands on the device.
# Setup
Build with `make` and you're good to go.
# Usage
 - Replace `.command` with the command you want to execute on the remote Telnet server, then build with `make`.
```c
static telnet_cmd_data tmp_get_backup_cmd = {
    .command            = "tftp -g -l sample_file 192.168.1.3 12345",
    .error_substr       = "tftp:",
};
```
I was using this tool for only one task (downloading sample\_file from the router), but you can easily extend this
program by reading the command from the file, for example.

 - Run `./exec_on_board` (probably on your device's LAN host)
```
-h, --help                                 Display this message.
-q, --quiet                                Produce no output.
-u, --username=<username>                  Specify username for telnet server. Default value is "admin".
-p, --password=<password>                  Specify password for telnet server. Default value is "admin".
-a, --addr=<[ipaddr][:port]>               Specify board address and telnet port. Default value is "192.168.1.1:23".
-t, --tftp-addr=<[ipaddr][:port]>          Specify address and port for tftp server. Default value is "192.168.1.3:12345".
    --tftp-dir=<dir>                       Specify directory for tftp server. Default value is ".".
    --cl-prompt=<str>                      Specify command line prompt. Default value is "root@rtr:~#".
    --login-prompt=<str>                   Specify login prompt. Default value is "login:".
    --pwd-prompt=<str>                     Specify password prompt. Default value is "Password:".
```
