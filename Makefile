LIBNAME=librebind.so
SRC=$(wildcard *.c)
OBJ=$(SRC:.c=.o)

CFLAGS=-O2 -fPIC

all:$(LIBNAME)

$(LIBNAME): $(OBJ)
	$(CC) -Wall -Wextra -shared -o $@  $^ -ldl
	strip --strip-all $@

clean:
	rm -f *.o *.so


.PHONY: all clean
