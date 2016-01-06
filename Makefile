CC = g++
CFLAGS = -std=c++11 -Wall -Wshadow -Wextra -g
DEPS = domain.h node.h cube.h coord.h
OBJ = generate.o domain.o node.o cube.o

all: draw gnuplot generate

draw: draw.cpp
	g++ -o $@ $< $(CFLAGS) `sdl-config --libs --cflags`

%.o: %.cpp $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

generate: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: zip-pngs

zip-pngs:
	rm -f bspline-pngs.zip
	zip -r bspline-pngs.zip png/