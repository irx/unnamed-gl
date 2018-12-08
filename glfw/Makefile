I_FLAGS = -I/usr/local/include
L_FLAGS = -L/usr/local/lib
LD_FLAGS = -lglfw -lGL -lm

example: example.o
	@${CC} -o example example.o ${L_FLAGS} ${LD_FLAGS}

example.o: example.c
	@${CC} -c example.c ${I_FLAGS}
