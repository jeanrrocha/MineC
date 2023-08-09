minecraft.exe: main.o mat.o
	g++ main.o mat.o -lm -o minecraft.exe

main.o: src/main.c lib/mat.h
	g++ src/main.c -c -lm

mat.o: lib/mat.c lib/mat.h
	g++ lib/mat.c -c -lm
