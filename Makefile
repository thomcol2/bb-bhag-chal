CFLAGS = ${INCLUDES} -Wall -g 
INCLUDES = -I./include
LIBS =  
SRC = $(wildcard src/*.c)
OUT = main

all: build run
build:
	gcc ${CFLAGS} -o ${OUT} ${SRC} ${LIBS} 
run:
	./${OUT}
clean:
	rm -f ${OUT}
