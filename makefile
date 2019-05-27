
bin/server:obj/server.o obj/serverfunc.o obj/threadpool.o
	gcc -o bin/server obj/server.o obj/serverfunc.o obj/threadpool.o -lpthread


bin/client:obj/client.o obj/clientfunc.o
	gcc -o bin/client obj/client.o obj/clientfunc.o


obj/server.o:src/server.c
	gcc -o obj/server.o -c  src/server.c

obj/serverfunc.o: src/serverfunc.c
	gcc -o obj/serverfunc.o -c  src/serverfunc.c

obj/threadpool.o:src/threadpool.c
	gcc -o obj/threadpool.o -c  src/threadpool.c

obj/client.o: src/client.c
	gcc -o obj/client.o -c src/client.c

obj/clientfunc.o: src/clientfunc.c
	gcc -o obj/clientfunc.o -c  src/clientfunc.c

clean:
	rm obj/* bin/*

