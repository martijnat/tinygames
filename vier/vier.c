//usr/bin/gcc $0 -o $0.bin -lSDL2 && ./$0.bin;exit
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#define FPS 60
#define GRID_PIXEL 24
#define GRID_BORDER 1
#define GRID_W 10
#define GRID_H 20
#define WIDTH (GRID_W * GRID_PIXEL)
#define HEIGHT (GRID_H * GRID_PIXEL)
#define NUM_PIECES 7
#define DROP_TIMEOUT (FPS/4)
#define MOVE_TIMEOUT (FPS/15)
#define ROTATE_TIMEOUT (FPS/10)

// Define tetromino shapes (I, J, L, O, S, T, Z)
const int shapes[NUM_PIECES][4][4][4] = {
  // I piece
  {
    {{0,0,0,0}, {1,1,1,1}, {0,0,0,0}, {0,0,0,0}},
    {{0,0,1,0}, {0,0,1,0}, {0,0,1,0}, {0,0,1,0}},
    {{0,0,0,0}, {0,0,0,0}, {1,1,1,1}, {0,0,0,0}},
    {{0,1,0,0}, {0,1,0,0}, {0,1,0,0}, {0,1,0,0}}
  },
  // J piece
  {
    {{1,0,0,0}, {1,1,1,0}, {0,0,0,0}, {0,0,0,0}},
    {{0,1,1,0}, {0,1,0,0}, {0,1,0,0}, {0,0,0,0}},
    {{0,0,0,0}, {1,1,1,0}, {0,0,1,0}, {0,0,0,0}},
    {{0,1,0,0}, {0,1,0,0}, {1,1,0,0}, {0,0,0,0}}
  },
  // L piece
  {
    {{0,0,1,0}, {1,1,1,0}, {0,0,0,0}, {0,0,0,0}},
    {{0,1,0,0}, {0,1,0,0}, {0,1,1,0}, {0,0,0,0}},
    {{0,0,0,0}, {1,1,1,0}, {1,0,0,0}, {0,0,0,0}},
    {{1,1,0,0}, {0,1,0,0}, {0,1,0,0}, {0,0,0,0}}
  },
  // O piece
  {
    {{0,0,0,0}, {0,1,1,0}, {0,1,1,0}, {0,0,0,0}},
    {{0,0,0,0}, {0,1,1,0}, {0,1,1,0}, {0,0,0,0}},
    {{0,0,0,0}, {0,1,1,0}, {0,1,1,0}, {0,0,0,0}},
    {{0,0,0,0}, {0,1,1,0}, {0,1,1,0}, {0,0,0,0}}
  },
  // S piece
  {
    {{0,0,0,0}, {0,1,1,0}, {1,1,0,0}, {0,0,0,0}},
    {{0,1,0,0}, {0,1,1,0}, {0,0,1,0}, {0,0,0,0}},
    {{0,0,0,0}, {0,1,1,0}, {1,1,0,0}, {0,0,0,0}},
    {{0,1,0,0}, {0,1,1,0}, {0,0,1,0}, {0,0,0,0}}
  },
  // T piece
  {
    {{0,0,0,0}, {0,1,0,0}, {1,1,1,0}, {0,0,0,0}},
    {{0,1,0,0}, {0,1,1,0}, {0,1,0,0}, {0,0,0,0}},
    {{0,0,0,0}, {1,1,1,0}, {0,1,0,0}, {0,0,0,0}},
    {{0,1,0,0}, {1,1,0,0}, {0,1,0,0}, {0,0,0,0}}
  },
  // Z piece
  {
    {{0,0,0,0}, {1,1,0,0}, {0,1,1,0}, {0,0,0,0}},
    {{0,0,1,0}, {0,1,1,0}, {0,1,0,0}, {0,0,0,0}},
    {{0,0,0,0}, {1,1,0,0}, {0,1,1,0}, {0,0,0,0}},
    {{0,1,0,0}, {1,1,0,0}, {1,0,0,0}, {0,0,0,0}}
  }
};

// Define colors for each piece type (RGB)
const int colors[NUM_PIECES][3] = {
  {142,192,124},   // I: cyan
  {131,165,152},     // J: blue
  {254,128,25},   // L: orange
  {250,189,47},   // O: yellow
  {184,187,38},     // S: green
  {211,134,155},   // T: purple
  {251,73,52}      // Z: red
};

void draw_pixel(SDL_Renderer* R, int x, int y, int r, int g, int b) {
  SDL_SetRenderDrawColor(R, (r*3)/4, (g*3)/4, (b*3)/4, 255);
  SDL_Rect pixel_border = {x, y, GRID_PIXEL, GRID_PIXEL};
  SDL_RenderFillRect(R, &pixel_border);
  SDL_SetRenderDrawColor(R, r, g, b, 255);
  SDL_Rect pixel = {x+2, y+2, GRID_PIXEL-4, GRID_PIXEL-4};
  SDL_RenderFillRect(R, &pixel);
}

int check_collision(int piece, int rotation, int x, int y, int grid[GRID_H][GRID_W]) {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (shapes[piece][rotation][i][j]) {
        int grid_x = x + j;
        int grid_y = y + i;
        if (grid_x < 0 || grid_x >= GRID_W || grid_y >= GRID_H)
          return 1;
        if (grid_y >= 0 && grid[grid_y][grid_x] != 0)
          return 1;
      }
    }
  }
  return 0;
}

void lock_piece(int piece, int rotation, int x, int y, int grid[GRID_H][GRID_W]) {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (shapes[piece][rotation][i][j]) {
        int grid_x = x + j;
        int grid_y = y + i;
        if (grid_y >= 0) {
          grid[grid_y][grid_x] = piece + 1;
        }
      }
    }
  }
}

int clear_lines(int grid[GRID_H][GRID_W]) {
  int lines_cleared = 0;
  for (int y = GRID_H - 1; y >= 0; y--) {
    int full = 1;
    for (int x = 0; x < GRID_W; x++) {
      if (grid[y][x] == 0) {
        full = 0;
        break;
      }
    }
    if (full) {
      for (int y2 = y; y2 > 0; y2--) {
        for (int x = 0; x < GRID_W; x++) {
          grid[y2][x] = grid[y2 - 1][x];
        }
      }
      for (int x = 0; x < GRID_W; x++) {
        grid[0][x] = 0;
      }
      lines_cleared++;
      y++; // Re-check same row position after shift
    }
  }
  return lines_cleared;
}

int main(int argc, char* argv[]) {
  srand(time(NULL));
  SDL_Init(SDL_INIT_VIDEO);
  SDL_Window* window = SDL_CreateWindow("vier",
                                        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                        WIDTH, HEIGHT, 0);
  SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  int grid[GRID_H][GRID_W] = {0}; // 0 = empty, 1-7 = piece type
  int input_timeout = 0;
  int current_piece = rand() % NUM_PIECES;
  int current_rotation = 0;
  int current_x = GRID_W / 2 - 2;
  int current_y = 0;
  int next_piece = rand() % NUM_PIECES;
  int gravity_counter = 0;
  int gravity_delay = FPS; // 1 second per block
  int total_lines = 0;
  int running = 1;

  while (running) {
    // Event handling
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) running = 0;
    }

    // Keyboard input
    const Uint8* keystate = SDL_GetKeyboardState(NULL);
    if(input_timeout<=0) {
      if (keystate[SDL_SCANCODE_LEFT] && !check_collision(current_piece, current_rotation, current_x - 1, current_y, grid))
        current_x--,input_timeout=MOVE_TIMEOUT;
      if (keystate[SDL_SCANCODE_RIGHT] && !check_collision(current_piece, current_rotation, current_x + 1, current_y, grid))
        current_x++,input_timeout=MOVE_TIMEOUT;
      if (keystate[SDL_SCANCODE_DOWN] && !check_collision(current_piece, current_rotation, current_x, current_y + 1, grid))
          current_y++,input_timeout=MOVE_TIMEOUT;
      if (keystate[SDL_SCANCODE_UP]) {
        int new_rotation = (current_rotation + 1) % 4;
        if (!check_collision(current_piece, new_rotation, current_x, current_y, grid)) {
            current_rotation = new_rotation;
            input_timeout=ROTATE_TIMEOUT;
          }
      }
      if (keystate[SDL_SCANCODE_SPACE]) {
        // Hard drop
        while (!check_collision(current_piece, current_rotation, current_x, current_y + 1, grid)) {
          current_y++;
        }
        lock_piece(current_piece, current_rotation, current_x, current_y, grid);
        input_timeout=DROP_TIMEOUT;
        total_lines += clear_lines(grid);
        current_piece = next_piece;
        next_piece = rand() % NUM_PIECES;
        current_rotation = 0;
        current_x = GRID_W / 2 - 2;
        current_y = 0;
        if (check_collision(current_piece, current_rotation, current_x, current_y, grid)) {
          running = 0; // Game over
        }
      }
    } else {
      input_timeout--;
    }

    // Gravity handling
    gravity_counter++;
    if (gravity_counter >= gravity_delay) {
      gravity_counter = 0;
      if (!check_collision(current_piece, current_rotation, current_x, current_y + 1, grid)) {
        current_y++;
      } else {
        lock_piece(current_piece, current_rotation, current_x, current_y, grid);
        total_lines += clear_lines(grid);
        current_piece = next_piece;
        next_piece = rand() % NUM_PIECES;
        current_rotation = 0;
        current_x = GRID_W / 2 - 2;
        current_y = 0;
        if (check_collision(current_piece, current_rotation, current_x, current_y, grid)) {
          running = 0; // Game over
        }
      }
    }

    // Drawing
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Draw locked pieces
    for (int y = 0; y < GRID_H; y++) {
      for (int x = 0; x < GRID_W; x++) {
        if (grid[y][x] != 0) {
          int piece = grid[y][x] - 1;
          draw_pixel(renderer, x * GRID_PIXEL, y * GRID_PIXEL,
                     colors[(piece+total_lines/10)%7][0], colors[(piece+total_lines/10)%7][1], colors[(piece+total_lines/10)%7][2]);
        } else {
          draw_pixel(renderer, x * GRID_PIXEL, y * GRID_PIXEL, 40, 40, 40);
        }
      }
    }

    // Draw current piece
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        if (shapes[current_piece][current_rotation][i][j]) {
          int x_pos = current_x + j;
          int y_pos = current_y + i;
          if (x_pos >= 0 && x_pos < GRID_W && y_pos >= 0 && y_pos < GRID_H) {
            draw_pixel(renderer, x_pos * GRID_PIXEL, y_pos * GRID_PIXEL,
                       colors[current_piece][0], colors[current_piece][1], colors[current_piece][2]);
          }
        }
      }
    }

    SDL_RenderPresent(renderer);
    SDL_Delay(1000 / FPS);
  }

  printf("Game Over! Total lines cleared: %d\n", total_lines);
  SDL_Delay(500);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
