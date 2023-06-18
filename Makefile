build-run:
	gcc -o asemu -DDEBUG -Wall src/*c
	./asemu

build:
	gcc -Wall src/*c -o asemu
