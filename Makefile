SRC = main.c
LIBS = 
OUT = main

all: build run
build:
	gcc -o ${OUT} ${SRC} ${LIBS} -g
release:
	gcc -O3 -o ${OUT} ${SRC} ${LIBS} -g
run:
	./${OUT}
