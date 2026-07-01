#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>

using namespace std;

const int SCREEN_W   = 640;
const int SCREEN_H   = 480;
const int BOX        = 20;
const int GAME_SPEED = 130;

SDL_Color COLOR_WHITE   = {255, 255, 255, 255};
SDL_Color COLOR_RED     = {255,  60,  60, 255};
SDL_Color COLOR_YELLOW  = {255, 220,   0, 255};
SDL_Color COLOR_GREEN   = {  0, 200,   0, 255};
SDL_Color COLOR_DKGREEN = {  0, 140,   0, 255};
SDL_Color COLOR_BLACK   = {  0,   0,   0, 255};

TTF_Font* findFont(int size) {
    vector<string> paths = {
        "font.ttf",
        "/usr/share/fonts/truetype/liberation/LiberationSans-Bold.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf",
        "/usr/share/fonts/TTF/DejaVuSans-Bold.ttf",
        "/usr/share/fonts/truetype/freefont/FreeSansBold.ttf",
        "/System/Library/Fonts/Helvetica.ttc",
        "C:/Windows/Fonts/arial.ttf"
    };

    for (int i = 0; i < (int)paths.size(); i++) {
        TTF_Font* f = TTF_OpenFont(paths[i].c_str(), size);
        if (f != nullptr) {
            cout << "Font পাওয়া গেছে: " << paths[i] << endl;
            return f;
        }
    }

    cout << "কোনো font পাওয়া যায়নি — text দেখাবে না" << endl;
    return nullptr;
}

void drawBox(SDL_Renderer* renderer, int x, int y, int size, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_Rect box = {x, y, size, size};
    SDL_RenderFillRect(renderer, &box);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &box);
}

void drawText(SDL_Renderer* renderer, TTF_Font* font,
              string text, int x, int y, SDL_Color color) {
    if (font == nullptr) return;
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
    if (surface == nullptr) return;
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect position    = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, nullptr, &position);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

int getTextWidth(TTF_Font* font, string text) {
    if (font == nullptr) return 0;
    int w = 0, h = 0;
    TTF_SizeText(font, text.c_str(), &w, &h);
    return w;
}

void drawTextCentered(SDL_Renderer* renderer, TTF_Font* font,
                      string text, int y, SDL_Color color) {
    int x = (SCREEN_W - getTextWidth(font, text)) / 2;
    drawText(renderer, font, text, x, y, color);
}

SDL_Point makeFood(vector<SDL_Point> snakeBody) {
    int totalCols = SCREEN_W / BOX;
    int totalRows = SCREEN_H / BOX;

    SDL_Point food;
    bool foodOnSnake = true;

    while (foodOnSnake) {
        food.x       = (rand() % totalCols) * BOX;
        food.y       = (rand() % totalRows) * BOX;
        foodOnSnake  = false;

        for (int i = 0; i < (int)snakeBody.size(); i++) {
            if (food.x == snakeBody[i].x && food.y == snakeBody[i].y) {
                foodOnSnake = true;
                break;
            }
        }
    }

    return food;
}

class Snake {
public:
    vector<SDL_Point> body;
    int dirX, dirY;
    int nextDirX, nextDirY;
    bool keyPressed;

    Snake(int startX, int startY) {
        dirX       =  1;
        dirY       =  0;
        nextDirX   =  1;
        nextDirY   =  0;
        keyPressed = false;

        body.push_back({startX,           startY});
        body.push_back({startX - BOX,     startY});
        body.push_back({startX - BOX * 2, startY});
    }

    void changeDir(int newX, int newY) {
        if (keyPressed) return;
        if (newX == -dirX && newY == -dirY) return;
        nextDirX   = newX;
        nextDirY   = newY;
        keyPressed = true;
    }

    void move() {
        dirX       = nextDirX;
        dirY       = nextDirY;
        keyPressed = false;

        SDL_Point newHead;
        newHead.x = body[0].x + dirX * BOX;
        newHead.y = body[0].y + dirY * BOX;

        body.insert(body.begin(), newHead);
        body.pop_back();
    }

    void grow() {
        SDL_Point lastPart = body.back();
        body.push_back(lastPart);
    }

    bool ateFood(SDL_Point food) {
        return (body[0].x == food.x && body[0].y == food.y);
    }

    bool hitWall() {
        if (body[0].x < 0)         return true;
        if (body[0].x >= SCREEN_W) return true;
        if (body[0].y < 0)         return true;
        if (body[0].y >= SCREEN_H) return true;
        return false;
    }

    bool hitSelf() {
        for (int i = 1; i < (int)body.size(); i++) {
            if (body[0].x == body[i].x && body[0].y == body[i].y) {
                return true;
            }
        }
        return false;
    }

    void draw(SDL_Renderer* renderer) {
        for (int i = 0; i < (int)body.size(); i++) {
            if (i == 0)
                drawBox(renderer, body[i].x, body[i].y, BOX, COLOR_DKGREEN);
            else
                drawBox(renderer, body[i].x, body[i].y, BOX, COLOR_GREEN);
        }
    }
};

int main(int argc, char* argv[]) {
    srand((unsigned int)time(nullptr));

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        cout << "SDL চালু হয়নি: " << SDL_GetError() << endl;
        return 1;
    }

    if (TTF_Init() != 0) {
        cout << "TTF চালু হয়নি: " << TTF_GetError() << endl;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Snake Game",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SCREEN_W, SCREEN_H,
        SDL_WINDOW_SHOWN
    );

    if (window == nullptr) {
        cout << "Window তৈরি হয়নি: " << SDL_GetError() << endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(
        window, -1, SDL_RENDERER_ACCELERATED
    );

    if (renderer == nullptr) {
        cout << "Renderer তৈরি হয়নি: " << SDL_GetError() << endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    TTF_Font* normalFont = findFont(22);
    TTF_Font* bigFont    = findFont(44);

    Snake     snake(SCREEN_W / 2, SCREEN_H / 2);
    SDL_Point food  = makeFood(snake.body);
    int       score = 0;
    bool      gameOver = false;
    bool      quit     = false;
    Uint32    lastMoveTime = SDL_GetTicks();
    SDL_Event event;

    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }

            if (event.type == SDL_KEYDOWN) {
                if (gameOver) {
                    if (event.key.keysym.sym == SDLK_r) {
                        snake        = Snake(SCREEN_W / 2, SCREEN_H / 2);
                        food         = makeFood(snake.body);
                        score        = 0;
                        gameOver     = false;
                        lastMoveTime = SDL_GetTicks();
                    }
                    if (event.key.keysym.sym == SDLK_ESCAPE) quit = true;
                } else {
                    if (event.key.keysym.sym == SDLK_UP)    snake.changeDir( 0, -1);
                    if (event.key.keysym.sym == SDLK_DOWN)  snake.changeDir( 0,  1);
                    if (event.key.keysym.sym == SDLK_RIGHT) snake.changeDir( 1,  0);
                }
            }
        }

        if (!gameOver) {
            Uint32 now = SDL_GetTicks();

            if (now - lastMoveTime >= GAME_SPEED) {
                lastMoveTime = now;
                snake.move();

                if (snake.hitWall() || snake.hitSelf()) {
                    gameOver = true;
                } else if (snake.ateFood(food)) {
                    snake.grow();
                    score = score + 10;
                    food  = makeFood(snake.body);
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_RenderClear(renderer);

        drawBox(renderer, food.x, food.y, BOX, COLOR_RED);
        snake.draw(renderer);
        drawText(renderer, normalFont, "Score: " + to_string(score), 10, 10, COLOR_WHITE);

        if (gameOver) {
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 160);
            SDL_Rect darkScreen = {0, 0, SCREEN_W, SCREEN_H};
            SDL_RenderFillRect(renderer, &darkScreen);

            drawTextCentered(renderer, bigFont,    "GAME OVER",                      SCREEN_H/2 - 80, COLOR_RED);
            drawTextCentered(renderer, normalFont, "Final Score: " + to_string(score), SCREEN_H/2,    COLOR_YELLOW);
            drawTextCentered(renderer, normalFont, "R = New Game        ESC = Quit", SCREEN_H/2 + 55, COLOR_WHITE);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(10);
    }

    if (normalFont) TTF_CloseFont(normalFont);
    if (bigFont)    TTF_CloseFont(bigFont);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
