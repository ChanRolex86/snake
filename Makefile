CC = gcc
CFLAGS = -Wall -std=c99 -I/opt/homebrew/include
LDFLAGS = -L/opt/homebrew/lib -lraylib -framework OpenGL -framework Cocoa -framework IOKit

SRC = main.c
OBJ = $(SRC:.c=.o)
EXEC = main

all: $(EXEC)

$(EXEC):
	$(CC) $(CFLAGS) $(SRC) -o $(EXEC) $(LDFLAGS)

clean:
	rm -rf $(EXEC)
	
.PHONY: all clean
