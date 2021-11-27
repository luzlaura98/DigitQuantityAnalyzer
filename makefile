all: e2.c
	gcc -c e2.c -lpthread
	gcc -o e2 e2.o -lpthread
clean:
	rm -f e2 e2.o