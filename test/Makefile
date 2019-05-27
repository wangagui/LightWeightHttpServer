SERVER = server
CLIENT = client
CC = gcc
CFLAGS = -std=cpp -Wall -I.

LINKER = gcc
LFLAGS = -Wall -I. -lm

SRCDIR = src
OBJDIR = obj
BINDIR = bin

CLIENTSRC:=$(src/client.c src/clientfunc.c)
SERVERSRC:=$(src/server.c src/serverfunc.c src/threadpool.c)
COBJECTS:=$(CLIENTSRC:.C=*.o)
SOBJECTS:=$(SERVERSRC:.c=*.o)
rm = rm -f

start: obj  
	gcc -o $(SERVER) $(SOBJECTS) -lpthread

	gcc -o $(CLIENT) $(COBJECTS)

obj:$(CLIENTSRC) $(SERVERSRC)
	gcc -o $(COBJECTS) -c $(CLIENTSRC)
	gcc -o $(SOBJECTS) -c $(SERVERSRC)

clean:
	rm $(SERVER) $(CLIENT) $(SOBJECTS) $(COBJECTS)



