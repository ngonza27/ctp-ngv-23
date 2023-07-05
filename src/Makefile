CC = gcc
CFLAGS = -Wall -Wextra -g -ffunction-sections -fdata-sections
LDFLAGS= -Wl,--gc-sections
all: main

main: dtc_decoder.c service01.c service02.c service0307.c sendMessage.c receiveMessage.c main.c

clean:
	rm -f main *.o
