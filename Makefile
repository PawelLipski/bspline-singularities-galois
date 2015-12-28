all: draw gnuplot generate

CPPFLAGS=-std=c++11 -Wall -Wshadow -Wextra -g

%: %.cpp
	g++ -o $@ $< $(CPPFLAGS)

generate: generate.cpp domain.cpp node.cpp cube.cpp
	g++ -o $@ $< $(CPPFLAGS)

draw: draw.cpp
	g++ -o $@ $< $(CPPFLAGS) `sdl-config --libs --cflags`

.PHONY: zip-pngs

zip-pngs:
	rm -f bspline-pngs.zip
	zip -r bspline-pngs.zip png/

