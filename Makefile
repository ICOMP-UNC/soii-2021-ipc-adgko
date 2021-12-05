FLAGS_GCC = -std=gnu11 -Wall -Werror -pedantic -Wextra -Wconversion -g -lzip
FLAGS_CPP = --enable=performance,portability,information,unusedFunction -q

all: check cliente server productores

clean:
	rm bin/*
	rm -r html/ latex/
	make clean_logs
	
clean_logs:
	echo " " > /home/diego/Escritorio/Sistemas-Operativos-2/TP1/soii-2021-ipc-adgko/archivos/logs/productor_1.log
	echo " " > /home/diego/Escritorio/Sistemas-Operativos-2/TP1/soii-2021-ipc-adgko/archivos/logs/productor_2.log
	echo " " > /home/diego/Escritorio/Sistemas-Operativos-2/TP1/soii-2021-ipc-adgko/archivos/logs/productor_3.log
	
check:
	cppcheck $(FLAGS_CPP) ./
	
cliente:
	gcc  src/sock_client.c -o bin/client $(FLAGS_GCC)
	
server:
	gcc  src/sock_server.c src/funciones1.c src/funciones2.c -o bin/DeliveryManager $(FLAGS_GCC)
	gcc  src/cli.c src/funciones1.c -o bin/cli $(FLAGS_GCC)
	gcc  src/sock_server_02.c src/funciones1.c src/funciones2.c -o bin/test_server $(FLAGS_GCC)
	
productores:
	gcc src/productor1.c src/funciones1.c -o bin/prod1 $(FLAGS_GCC)
	gcc src/productor2.c src/funciones1.c -o bin/prod2 $(FLAGS_GCC)
	gcc src/productor3.c src/funciones1.c -o bin/prod3 $(FLAGS_GCC)
	
docs:
	doxygen Doxyfile
