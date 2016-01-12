include config.mk

all: tetris

tetris: tetris.o plot.o
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	rm -f tetris
	find -name "*.o" -delete

.PHONY: all clean
