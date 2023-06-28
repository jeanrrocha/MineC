minecraft.exe: main.o mat.o
	gcc main.o mat.o -lm -o minecraft.exe

main.o: src/main.c lib/mat.h
	gcc src/main.c -c -lm

mat.o: lib/mat.c lib/mat.h
	gcc lib/mat.c -c -lm
