#include <iostream>
#include <SDL/SDL.h>
using namespace std;

SDL_Surface* screen;

#define BLACK 0x000000
#define RED   0xff0000
#define GREEN 0x00ff00
#define BLUE  0x0000ff
#define WHITE 0xffffff

void draw_element(int x, int y, int w, int h, int scale, int contour, int num, int lvl) {
	SDL_Rect rect = { Sint16(x*scale), Sint16(y*scale), Uint16(w*scale), Uint16(h*scale) };

	Uint32 color;
	enum {
		FULL, EDGE, VERTEX
	} type;

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

bool happened(SDL_Event& event, int key) {
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
	for (int i = 0; i < 500; i++) {
		int x = x1 + (x2 - x1) * i / 500;
		int y = y1 + (y2 - y1) * i / 500;
		put_pixel(x, y, BLUE);
	}
}

int main() {
	SDL_Init(SDL_INIT_VIDEO);
	screen = SDL_SetVideoMode(512, 512, 0, SDL_ANYFORMAT);
	SDL_WM_SetCaption("Esc to exit", NULL);
	SDL_FillRect(screen, NULL, BLACK);

	int N;
	cin >> N;
	for (int i = 0; i < N; i++) {
		int left, right, up, down, num, lvl;
		cin >> left >> right >> up >> down >> num >> lvl;
		int w = right - left;
		int h = down - up;
		draw_element(left, up, w, h, 8, 2, num, lvl);
        //if (num != -1){
        //    cout << num << endl;
        //}
        //SDL_Delay(100);
        //SDL_Flip(screen);
	}

	for (int i = 0; i < 256; i++) {
		int left, up, right, down;
		cin >> left >> up >> right >> down;
		draw_line(left, up, right, down, 8);
	}

	SDL_Flip(screen);
	//SDL_Delay(3000);

	/*
	int M;
	cin >> M;
	for (int i = 0; i < M; i++) {
		int left, right, up, down, num, lvl;
		cin >> left >> right >> up >> down >> num >> lvl;
		int w = right - left;
		int h = down - up;
		draw_element(left, up, w, h, 16, 0, num, lvl);
		SDL_Delay(1000);
		SDL_Flip(screen);
	}
	*/

	while (true) {
		SDL_Event event;
		if (SDL_PollEvent(&event) && happened(event, SDLK_ESCAPE))
			break;
		SDL_Delay(200);
	}
	SDL_Quit();
}

