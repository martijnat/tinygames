//usr/bin/gcc $0 -o $0.bin -lSDL2 && ./$0.bin;exit
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <time.h>


#define FPS 15
#define GRID_PIXEL 16
#define GRID_BORDER 1
#define GRID_W 40
#define GRID_H 30
#define WIDTH (GRID_W*GRID_PIXEL)
#define HEIGHT (GRID_H*GRID_PIXEL)
#define PLAYER_SIZE GRID_PIXEL
#define OBJECT_SIZE GRID_PIXEL
#define SPEED GRID_PIXEL
#define SNAKE_MAXLEN GRID_W*GRID_H

void draw_pixel(SDL_Renderer* R,int x,int y,int r,int g,int b)
{
        SDL_SetRenderDrawColor(R, (r*3)/4, (g*3)/4, (b*3)/4, 255);
        SDL_Rect pixel_border = {x, y, GRID_PIXEL, GRID_PIXEL};
        SDL_RenderFillRect(R, &pixel_border);
        SDL_SetRenderDrawColor(R, r, g, b, 255);
        SDL_Rect pixel = {x+2, y+2, GRID_PIXEL-4, GRID_PIXEL-4};
        SDL_RenderFillRect(R, &pixel);
}

int main(int argc, char* argv[]) {
        SDL_Init(SDL_INIT_VIDEO);

        int head_x=3*GRID_PIXEL;
        int head_y=3*GRID_PIXEL;
        int dx=1;
        int dy=0;
        int snakelen=1;
        int snake_x[SNAKE_MAXLEN];
        int snake_y[SNAKE_MAXLEN];
        snake_x[0]=head_x;
        snake_y[0]=head_y;

        srand(time(NULL));
        int food_x = GRID_PIXEL*(1+(rand())%(GRID_W-2));
        int food_y= GRID_PIXEL*(1+(rand())%(GRID_H-2));
        SDL_Window* window = SDL_CreateWindow("snek",
                                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                              WIDTH, HEIGHT, 0);

        SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

        int running = 1;
        const Uint8* keystate;

        while (running) {
                // Event handling
                SDL_Event event;
                while (SDL_PollEvent(&event)) {
                        if (event.type == SDL_QUIT) running = 0;
                }

                // Keyboard input
                keystate = SDL_GetKeyboardState(NULL);
                if (keystate[SDL_SCANCODE_LEFT] && dx==0) {dx=-1;dy=0;}
                if (keystate[SDL_SCANCODE_RIGHT] && dx==0) {dx=1;dy=0;}
                if (keystate[SDL_SCANCODE_UP] && dy==0) {dx=0;dy=-1;}
                if (keystate[SDL_SCANCODE_DOWN] && dy==0) {dx=0;dy=1;}

                head_x+=dx*GRID_PIXEL;
                head_y+=dy*GRID_PIXEL;

                // Boundary check
                if ((head_x < 0) || (head_y < 0) ||
                    (head_x >= WIDTH - GRID_PIXEL) ||
                    (head_y >= HEIGHT - GRID_PIXEL) )running =0;

                for(int i=snakelen-1;i>0;i--)
                {
                        if(head_x==snake_x[i] && head_y==snake_y[i])running=0;
                        snake_x[i]=snake_x[i-1];
                        snake_y[i]=snake_y[i-1];
                }
                snake_x[0]=head_x;
                snake_y[0]=head_y;

                if( food_x==head_x && food_y==head_y)
                {
                        snakelen+=1;
                        snake_x[snakelen-1]=head_x;
                        snake_y[snakelen-1]=head_y;
                        food_x = GRID_PIXEL*(1+(rand())%(GRID_W-2));
                        food_y= GRID_PIXEL*(1+(rand())%(GRID_H-2));
                }

                for(int x=0;x<WIDTH;x+=GRID_PIXEL)
                        for(int y=0;y<HEIGHT;y+=GRID_PIXEL)
                                draw_pixel(renderer,x,y,33,33,33);
                for(int i=0;i<snakelen;i++)
                {
                        draw_pixel(renderer,snake_x[i], snake_y[i],0,200,0);
                }
                draw_pixel(renderer,food_x, food_y,200,0,0);

                SDL_RenderPresent(renderer);
                SDL_Delay(1000/FPS);
        }
        printf("score: %d\n",snakelen);
}
