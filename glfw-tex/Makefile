I_FLAGS = -I/usr/local/include
L_FLAGS = -L/usr/local/lib
LD_FLAGS = -lglfw -lGL -lm -ljpeg

tex: main.o libimgio.o
	@${CC} -o tex main.o libimgio.o ${L_FLAGS} ${LD_FLAGS}

main.o: main.c
	@${CC} -c main.c ${I_FLAGS}

libimgio.o: libimgio.c
	@${CC} -c libimgio.c ${I_FLAGS}
