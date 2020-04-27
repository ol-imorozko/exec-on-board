all: tool

tool: dump_wifi_params.o args_check.o tcp_connection.o telnet_remote_control.o
	gcc dump_wifi_params.o args_check.o tcp_connection.o telnet_remote_control.o -o dump_wifi_params

dump_wifi_params.o: dump_wifi_params.c
	gcc -Wall -Wextra -c dump_wifi_params.c -o dump_wifi_params.o

args_check.o: args_check.c
	gcc -Wall -Wextra -c args_check.c -o args_check.o

tcp_connection.o: tcp_connection.c
	gcc -Wall -Wextra -c tcp_connection.c -o tcp_connection.o

telnet_remote_control.o: telnet_remote_control.c
	gcc -Wall -Wextra -c telnet_remote_control.c -o telnet_remote_control.o

clean:
	rm -r *.o tool
