FLAGS_GCC = -std=gnu11 -Wall -Werror -pedantic -Wextra -Wconversion
FLAGS_CPP = --enable=performance,portability,information,unusedFunction -q

all: check cliente server productores

clean:
	rm bin/*
	rm -r html/ latex/
	
check:
	cppcheck $(FLAGS_CPP) ./
	
cliente:
	gcc  src/sock_client.c -o bin/client $(FLAGS_GCC)
	
server:
	gcc  src/sock_server.c -o bin/DeliveryManager $(FLAGS_GCC)
	
productores:
	gcc src/productor1.c -o bin/prod1 $(FLAGS_GCC)
	gcc src/productor2.c -o bin/prod2 $(FLAGS_GCC)
	gcc src/productor3.c -o bin/prod3 $(FLAGS_GCC)
	
docs:
	doxygen Doxyfile
