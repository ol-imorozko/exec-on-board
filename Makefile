all: tool

tool: dump_wifi_params.o
	gcc dump_wifi_params.o -o dump_wifi_params

dump_wifi_params.o: dump_wifi_params.c
	gcc -Wall -Wextra -c dump_wifi_params.c -o dump_wifi_params.o

clean:
	rm -r *.o tool
