#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <string>
const int WINDOW_WIDTH  = 640;
const int WINDOW_HEIGHT = 480;

const int CELL_SIZE     = 20;
const int GRID_WIDTH    = WINDOW_WIDTH  / CELL_SIZE;
const int GRID_HEIGHT   = WINDOW_HEIGHT / CELL_SIZE;

enum Direction { UP, DOWN, LEFT, RIGHT };

class Snake {
public:
    Snake() {
       
        body.push_back({GRID_WIDTH/2, GRID_HEIGHT/2});
        body.push_back({GRID_WIDTH/2 - 1, GRID_HEIGHT/2});
        body.push_back({GRID_WIDTH/2 - 2, GRID_HEIGHT/2});
        direction = RIGHT;
        nextDirection = RIGHT;
    }

