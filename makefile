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

networking.o: networking.c
	@gcc -c networking.c networking.h gameFunctions.h

clean:
	rm *.o
	rm usedWords
	rm checkFile
	rm ./client
	rm ./server

# default:
# compile runme: gameFunctions.o gameFunctions.h
# 	@gcc gameFunctions.o gameFunctions.h -o runme
# run: runme
# 	@./runme $(ARGS)
# clean: 
# 	@rm -rf gameFunctions.o
# 	@rm -rf runme
# 	@rm -rf usedWords
# 	@rm -rf checkFile 
# gameFuctions.o:gameFunctions.c
# 	@gcc gameFunctions.c -c