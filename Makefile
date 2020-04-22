all: tool

tool: dump_wifi_params.o args_check.o
	gcc dump_wifi_params.o args_check.o -o dump_wifi_params

dump_wifi_params.o: dump_wifi_params.c
	gcc -Wall -Wextra -c dump_wifi_params.c -o dump_wifi_params.o

args_check.o: args_check.c
	gcc -Wall -Wextra -c args_check.c -o args_check.o

clean:
	rm -r *.o tool
