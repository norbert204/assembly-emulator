build-run:
	gcc -Wall -DDEBUG src/*.c src/base/*.c src/ram_file/*.c -o asemu
	./asemu

build:
	gcc -Wall -DDEBUG src/*.c src/base/*.c src/ram_file/*.c -o asemu
