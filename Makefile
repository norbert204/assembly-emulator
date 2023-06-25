build-run:
	gcc -Wall src/*.c src/base/*.c src/ram_file/*.c
	./a.out

build:
	gcc -Wall src/*.c src/base/*.c src/ram_file/*.c
