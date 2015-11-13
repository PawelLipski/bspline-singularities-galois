all: gnuplot tree draw bsplines

CPPFLAGS=-std=c++11 -Wall -Wshadow -Wextra

%: %.cpp
	g++ -o $@ $< $(CPPFLAGS)

draw: draw.cpp
	g++ -o $@ $< $(CPPFLAGS) `sdl-config --libs --cflags`

