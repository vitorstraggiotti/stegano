
CC = gcc

RELEASE_FLAGS = -c -Wall -pedantic -O2
DEBUG_FLAGS = -c -g -Wall -pedantic

PROGNAME = steg

.PHONY: all clean

all:
	@echo "Make options:"
	@echo "make $(PROGNAME)   --> build program"
	@echo "make $(PROGNAME)_d --> build debug version"
	@echo "make clean  --> clear files from build process"


# Building release version
$(PROGNAME): main.o bitmap.o
	$(CC) -o $@ $^

main.o: main.c
	$(CC) $(RELEASE_FLAGS) -o $@ $^

bitmap.o: bitmap.c
	$(CC) $(RELEASE_FLAGS) -o $@ $^

# Building debug version
$(PROGNAME)_d: main_d.o bitmap_d.o
	$(CC) -o $@ $^

main_d.o: main.c
	$(CC) $(DEBUG_FLAGS) -o $@ $^

bitmap_d.o: bitmap.c
	$(CC) $(DEBUG_FLAGS) -o $@ $^

clean:
	rm $(PROGNAME) $(PROGNAME)_d *.o
