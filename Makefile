all: tool

tool: dump_wifi_params.o args_check.o connection.o telnet_remote_control.o tftp_server.o
	gcc -pthread dump_wifi_params.o args_check.o connection.o telnet_remote_control.o tftp_server.o -o dump_wifi_params

dump_wifi_params.o: dump_wifi_params.c
	gcc -Wall -Wextra -c dump_wifi_params.c -o dump_wifi_params.o

args_check.o: args_check.c
	gcc -Wall -Wextra -c args_check.c -o args_check.o

connection.o: connection.c
	gcc -Wall -Wextra -c connection.c -o connection.o

telnet_remote_control.o: telnet_remote_control.c
	gcc -Wall -Wextra -c telnet_remote_control.c -o telnet_remote_control.o

tftp_server.o: tftp_server.c
	gcc -Wall -Wextra -c tftp_server.c -o tftp_server.o

clean:
	rm -r *.o tool
