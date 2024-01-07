default:

compile: client.o server.o networking.o gameFunctions.o
	@gcc -o client client.o networking.o gameFunctions.o
	@gcc -o server server.o networking.o gameFunctions.o
#	@gcc -o runme gameFunctions.o networking.o

client: compile
	@./client $(ARGS)

server: compile
	@./server

client.o: client.c networking.h
	@gcc -c client.c networking.h gameFunctions.h

server.o: server.c networking.h
	@gcc -c server.c networking.h gameFunctions.h

gameFunctions.o: gameFunctions.c
	@gcc -c gameFunctions.c networking.h gameFunctions.h

clean:
	rm *.o
	rm ./client
	rm ./server