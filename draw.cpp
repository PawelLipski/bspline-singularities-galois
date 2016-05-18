#include <algorithm>
#include <cstdio>
#include <iostream>
#include <SDL/SDL.h>
#include <vector>
using namespace std;

SDL_Surface* screen;

#define BLACK 0x000000
#define RED   0xff0000
#define MGNTA 0xffff00
#define GREEN 0x00ff00
#define BLUE  0x0000ff
#define WHITE 0xffffff

//vector<SDL_Rect> rects;

void print_rect(const SDL_Rect& rect) {
	cout << rect.x << ", " << rect.y << " => " << rect.x << " x " << rect.y << endl;
}

enum Type {
	FULL, EDGE, VERTEX
};

void draw_element(int x, int y, int w, int h, int scale, int contour) {
	SDL_Rect rect = { Sint16(x*scale), Sint16(y*scale), Uint16(w*scale), Uint16(h*scale) };

	Type type;

	Uint32 color;
	if (w == 0 && h == 0) {
		type = VERTEX;
		rect.x -= 2;
		rect.y -= 2;
		rect.w = 4;
		rect.h = 4;
		color = BLUE;
	} else if (w == 0 || h == 0) {
		type = EDGE;
		(w == 0 ? rect.w : rect.h) = 1;
		color = WHITE;
	} else {
		rect.x++;
		rect.y++;
		rect.w -= 2;
		rect.h -= 2;
		type = FULL;
		color = RED;
	}
	SDL_FillRect(screen, &rect, color);

	if (type == FULL) {
		rect.x += contour;
		rect.y += contour;
		rect.w -= contour * 2;
		rect.h -= contour * 2;
		SDL_FillRect(screen, &rect, BLACK);
	}
}

void draw_element_clear(int x, int y, int w, int h, int scale, int contour) {
	SDL_Rect rect = { Sint16(x*scale), Sint16(y*scale), Uint16(w*scale), Uint16(h*scale) };

	Uint32 color;
	Type type;

	if (w == 0 && h == 0) {
		/*type = VERTEX;
		rect.x -= 2;
		rect.y -= 2;
		rect.w = 4;
		rect.h = 4;
		color = BLUE;*/
	} else if (w == 0 || h == 0) {
		type = EDGE;
		(w == 0 ? rect.w : rect.h) = 2;
		(w == 0 ? rect.x : rect.y) += 4;
		color = BLACK;
	} else {
		//rect.x++;
		//rect.y++;
		//rect.w -= 2;
		//rect.h -= 2;
		type = FULL;
		color = BLACK;
	}
	SDL_FillRect(screen, &rect, color);

	if (type == FULL) {
		rect.x += contour;
		rect.y += contour;
		rect.w -= contour * 2;
		rect.h -= contour * 2;
		SDL_FillRect(screen, &rect, WHITE);
	}
}

bool was_key_up(SDL_Event& event, int key) {
	return event.type == SDL_KEYUP && event.key.keysym.sym == key;
}

void put_pixel(int x, int y, Uint32 color) {
	SDL_Rect rect = { (Sint16)x, (Sint16)y, (Uint16)1, (Uint16)1 };
	SDL_FillRect(screen, &rect, color);
}

void draw_line(int x1, int y1, int x2, int y2, int scale) {
	x1 *= scale;
	y1 *= scale;
	x2 *= scale;
	y2 *= scale;
	for (int i = 50; i < 450; i++) {
		int x = x1 + (x2 - x1) * i / 500;
		int y = y1 + (y2 - y1) * i / 500;
		put_pixel(x, y, BLUE);
	}
}

bool wait_until_key_or_quit(int key) {
	while (true) {
		SDL_Event event;
		int res = SDL_PollEvent(&event);
		if (res && was_key_up(event, key)) {
			return false;
		}
		if (res && was_key_up(event, SDLK_ESCAPE)) {
			return true;
		}
		SDL_Delay(200);
	}
}

struct RectDef {
	int x, y, w, h;
};

vector<RectDef> rects;
int scale;

void redraw() {
	SDL_FillRect(screen, NULL, BLACK);
	for (auto& rect: rects) {
		draw_element(rect.x, rect.y, rect.w, rect.h, scale, 2);
	}
}

void redraw_clear() {
	SDL_FillRect(screen, NULL, WHITE);
	for (auto& rect: rects) {
		draw_element_clear(rect.x, rect.y, rect.w, rect.h, scale, 1);
	}
}

void draw_inside(int index) {
	const auto& r = rects[index];
	SDL_Rect inside = { Sint16(r.x*scale + 3), Sint16(r.y*scale + 3), Uint16(r.w*scale - 6), Uint16(r.h*scale - 6) };
	SDL_FillRect(screen, &inside, GREEN);
}

int main(int argc, char** argv) {
	const int SIZE = 512;

	enum InputFormat {
		PLAIN,
		NEIGHBORS,
		SUPPORTS,
		CLEAR,
		MATRIX
	} input_format = PLAIN;

	if (argc >= 2) {
		bool any_format = true;
		string opt(argv[1]);
		if (opt == "-c" || opt == "--clear")
			input_format = CLEAR;
		else if (opt == "-m" || opt == "--matrix")
			input_format = MATRIX;
		else if (opt == "-n" || opt == "--neighbors")
			input_format = NEIGHBORS;
		else if (opt == "-s" || opt == "--supports")
			input_format = SUPPORTS;
		else
			any_format = false;
		if (any_format) {
			argc--;
			argv++;
		}
	}

	int depth = argc == 1 ? 3 : atoi(argv[1]);
	if (input_format == NEIGHBORS)
		scale = 32 >> depth;
	else
		scale = 256 >> depth;


	SDL_Init(SDL_INIT_VIDEO);
	screen = SDL_SetVideoMode(SIZE, SIZE, 0, SDL_ANYFORMAT);
	SDL_WM_SetCaption("Esc to exit", NULL);

	int N;
	cin >> N;
	for (int i = 0; i < N; i++) {
		int left, right, up, down;
		cin >> left >> right >> up >> down;

		int w = right - left;
		int h = down - up;
		RectDef rect = { left, up, w, h };
		rects.push_back(rect);
	}
	if (input_format == CLEAR) {
		redraw_clear();
		char bmp[100];
		sprintf(bmp, "mesh-%i.bmp", depth);
		SDL_SaveBMP(screen, bmp);
	} else {
		redraw();
	}

	if (input_format == NEIGHBORS) {
		int M;
		cin >> M;
		for (int i = 0; i < M; i++) {
			int left, up, right, down;
			cin >> left >> up >> right >> down;
			draw_line(left, up, right, down, scale);
		}
	} else if (input_format == SUPPORTS) {
		int M;
		cin >> M;
		for (int i = 0; i < M; i++) {
			int x, y, cnt;
			cin >> x >> y >> cnt;
			cout << i << endl;
			for (int j = 0; j < cnt; j++) {
				int index;
				cin >> index;
				draw_inside(index);
			}
			SDL_Rect mid = { Sint16(x*scale - 3), Sint16(y*scale - 3), 6, 6};
			SDL_FillRect(screen, &mid, MGNTA);
			SDL_Flip(screen);
			if (wait_until_key_or_quit(SDLK_SPACE)) {
				SDL_Quit();
				return 0;
			}
			redraw();
		}
	} else if (input_format == MATRIX) {
		int M;
		cin >> M;
		vector<vector<int>> supports(M);
		for (int i = 0; i < M; i++) {
			int x, y, cnt;
			cin >> x >> y >> cnt;		
			for (int j = 0; j < cnt; j++) {
				int index;
				cin >> index;
				supports[i].push_back(index);
			}
		}
		for (int i = 0; i < M; i++) {
			for (int j = 0; j < M; j++) {
				vector<int> common(M);
				auto iter = set_intersection(
					supports[i].begin(), supports[i].end(),
					supports[j].begin(), supports[j].end(),
					common.begin());
				cout << (iter == common.begin() ? "." : "X");
			}
			cout << endl;
		}
	}

	SDL_Flip(screen);
	wait_until_key_or_quit(SDLK_ESCAPE);
	SDL_Quit();
	return 0;
}

