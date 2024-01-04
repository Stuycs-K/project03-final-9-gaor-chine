default:
compile runme: gameFunctions.o gameFunctions.h
	@gcc gameFuctions.o gameFunctions.h -o runme
run: runme
	@./runme
clean: 
	@rm -rf gameFunctions.o
	@rm -rf runme
gameFuctions.o:gameFunctions.c
	@gcc gameFunctions.c -c