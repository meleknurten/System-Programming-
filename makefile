all: 
	clear
	gcc -c serverp.c
	gcc serverp.c restart.c uici.c uiciname.c -o serverp -pthread
	gcc -c client.c
	gcc client.c restart.c uici.c uiciname.c -o client -pthread

