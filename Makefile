CC = g++
CPPFLAGS = -std=c++11 -Wall -Wshadow -Wextra -g
DEPS = domain.h node.h cube.h coord.h
OBJ = generate.o domain.o node.o cube.o
PROGRAMS = draw gnuplot generate

all: $(PROGRAMS)

gnuplot: gnuplot.cpp cube.o
	$(CC) -o $@ $^ $(CPPFLAGS)

draw: draw.cpp
	$(CC) -o $@ $^ $(CPPFLAGS) `sdl-config --libs --cflags`

%.o: %.cpp $(DEPS)
	$(CC) $(CPPFLAGS) -c -o $@ $<

generate: $(OBJ)
	$(CC) $(CPPFLAGS) -o $@ $^

.PHONY: zip-pngs clean

zip-pngs:
	rm -f bspline-pngs.zip
	zip -r bspline-pngs.zip png/

clean:
	@rm -f $(PROGRAMS) *.o
