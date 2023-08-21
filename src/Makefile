main.exe: main.c avl.h
	g++ main.c ../lib/avl.c -o main.exe -lm -march=x86-64 -lglfw -lGL -lGLEW -I ../include/ -L ../lib/
	
	#-lglfw -lGL -lGLEW
	#-I ../include/ -L ../lib/
	##-I ../lib/glew-2.2.0/include -I ../lib/glfw-3.3.8.bin.WIN64/include -L ../lib/glew-2.2.0/lib -L ../lib/glfw-3.3.8.bin.WIN64/lib-vc2022

#avl.o: avl.c avl.h
#	g++ avl.c -g -c -lglfw -lGL -lGLEW -lm