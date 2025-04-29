main: shell.c helpers.c external.c pipeExternal.c
	gcc shell.c helpers.c external.c pipeExternal.c -o shell -Wall -Werror


clean:
	rm -rf *.o shell.out	