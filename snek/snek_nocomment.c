//usr/bin/gcc $0 -o $0.bin -lSDL2 && ./$0.bin;exit
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <time.h>

void draw_pixel(SDL_Renderer* R,int x,int y,int r,int g,int b)
{
        SDL_SetRenderDrawColor(R, (r*3)/4, (g*3)/4, (b*3)/4, 255);
        SDL_Rect pixel_border = {x, y, 16, 16};
        SDL_RenderFillRect(R, &pixel_border);
        SDL_SetRenderDrawColor(R, r, g, b, 255);
        SDL_Rect pixel = {x+2, y+2, 16-4, 16-4};
        SDL_RenderFillRect(R, &pixel);
}

int main(int argc, char* argv[]) {
        SDL_Init(SDL_INIT_VIDEO);

        int head_x=0;
        int head_y=0;
        int dx=1;
        int dy=0;
        int snakelen=1;
        int snake_x[1200];
        int snake_y[1200];
        snake_x[0]=snake_y[0]=0;
        srand(time(0));
        int food_x = 16+16*(rand()%38);
        int food_y = 16+16*(rand()%28);
        SDL_Window* window = SDL_CreateWindow("snek", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, 0);
        SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        int running = 1;
        const Uint8* keystate;
        while (running) {
                SDL_Event event;
                while (SDL_PollEvent(&event)){if (event.type == SDL_QUIT) running = 0;}
                keystate = SDL_GetKeyboardState(0);
                if (keystate[SDL_SCANCODE_LEFT] && dx==0) {dx=-1;dy=0;}
                if (keystate[SDL_SCANCODE_RIGHT] && dx==0) {dx=1;dy=0;}
                if (keystate[SDL_SCANCODE_UP] && dy==0) {dx=0;dy=-1;}
                if (keystate[SDL_SCANCODE_DOWN] && dy==0) {dx=0;dy=1;}
                head_x+=dx*16;
                head_y+=dy*16;
                if ((head_x < 0) || (head_y < 0) || (head_x >= 640 - 16) || (head_y >= 480 - 16) )running =0;
                for(int i=snakelen-1;i>0;i--)
                        if(head_x==snake_x[i] && head_y==snake_y[i])
                                running=0;
                        else
                                snake_x[i]=snake_x[i-1],snake_y[i]=snake_y[i-1];
                snake_x[0]=head_x;
                snake_y[0]=head_y;

                if( food_x==head_x && food_y==head_y)
                        snakelen+=1, snake_x[snakelen-1]=head_x, snake_y[snakelen-1]=head_y, food_x = 16+16*(rand()%38), food_y = 16+16*(rand()%28);

                for(int x=0;x<640;x+=16)
                        for(int y=0;y<480;y+=16)
                                draw_pixel(renderer,x,y,33,33,33);
                for(int i=0;i<snakelen;i++)
                        draw_pixel(renderer,snake_x[i], snake_y[i],0,200,0);
                draw_pixel(renderer,food_x, food_y,200,0,0);
                SDL_RenderPresent(renderer);
                SDL_Delay(65);
        }
        printf("score: %d\n",snakelen);
}
