interpreter: interpreter.o 
	clang interpreter.o -o interpreter
	
interpreter.o: interpreter.c
	clang -c interpreter.c


clean:
	rm *.o interpreter
	