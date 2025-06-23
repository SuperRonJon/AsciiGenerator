build/asciigen: build/main.o 
	gcc -O2 -o build/asciigen build/main.o -lm

build/main.o: main.c stb_image.h stb_image_resize2.h
	gcc -O2 -c -Wall -Wextra -o build/main.o main.c

debug: build/debug

build/debug: main.c stb_image.h stb_image_resize2.h
	gcc -Wall -Wpedantic -Wextra -o build/debug main.c -lm -g

all: build/asciigen build/debug
