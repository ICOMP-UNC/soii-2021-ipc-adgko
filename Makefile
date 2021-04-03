FLAGS_GCC = -std=gnu11 -Wall -Werror -pedantic -Wextra -Wconversion
FLAGS_CPP = --enable=performance,portability,information,unusedFunction -q

make:
	cppcheck $(FLAGS_CPP) ./
	gcc  src/sock_server.c -o bin/server $(FLAGS_GCC)
	gcc  src/sock_client.c -o bin/client $(FLAGS_GCC)

clean:
	rm bin/*
	#rm -r html/ latex/
