test: test.o render.o ../ff/ff.o
	clang -o test test.o render.o ../ff/ff.o -L/usr/local/lib -lGL -lglfw -lGLEW -lm

render.o: render.c
	clang -c render.c -I/usr/local/include
