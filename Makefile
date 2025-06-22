build/asciigen: build/main.o build/config.o build/ascgen.o
	gcc -O2 -o build/asciigen build/main.o build/config.o build/ascgen.o -lm

build/main.o: main.c
	gcc -O2 -c -o build/main.o main.c

build/config.o: config.c config.h
	gcc -O2 -c -o build/config.o config.c

build/ascgen.o: ascgen.c ascgen.h
	gcc -O2 -c -o build/ascgen.o ascgen.c

debug: build/debug

build/debug: main.c
	gcc -o build/debug main.c config.c ascgen.c -lm -g

all: build/asciigen build/debug
