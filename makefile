CFLAGS = -Wall -Wextra -std=c99

all: OCR

OCR: main.o Bmp_Parser.o RLSA.o cut.o Xor
	gcc `pkg-config --cflags gtk+-3.0` *.o -o OCR `pkg-config --libs gtk+-3.0 ` -lSDL2 -lSDL2_image

main.o: main.c
	gcc -c main.c `pkg-config --cflags --libs gtk+-3.0`

Bmp_Parser.o: Bmp_Parser.c
	gcc -c Bmp_Parser.c 

#image.o: type/image.c
	#gcc -c type/image.c

RLSA.o: RLSA.c
	gcc -c RLSA.c

cut.o: cut.c
	gcc -c cut.c

Xor: Xor.c
	gcc Xor.c -o Xor -lm

clean:
	rm *.o;
	rm OCR;
	rm Xor;