default:
compile: client.o server.o networking.o
	@gcc -o client client.o networking.o
	@gcc -o server server.o networking.o
	@gcc -o gameFunctions.o gameFunctions.h -o runme

client: compile
	@./client $(ARGS)

server: compile
	@./server

client.o: client.c networking.h
	@gcc -c client.c networking.h

server.o: server.c networking.h
	@gcc -c server.c networking.h

gameFuctions.o:gameFunctions.c
	@gcc gameFunctions.c -c

clean:
	rm *.o
	rm ./client
	rm ./server