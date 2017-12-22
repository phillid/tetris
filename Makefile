include config.mk

all: tetris

tetris: tetris.o plot.o

clean:
	rm -f tetris
	find . -name "*.o" -exec rm "{}" \;

.PHONY: all clean
