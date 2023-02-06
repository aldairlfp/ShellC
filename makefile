default: build

build:
	gcc -o shell shell.h shell.c

run: build
	./shell