#ifndef SNAKE_H
#define SNAKE_H

#include <deque>
#include <ctime>
#include <cstdio>
#include <math.h>
#include <ncurses.h>
#include <unistd.h>

#define HEIGHT 58
#define WIDTH 238

using namespace std;

class SnakeGame {
    public:
        deque<int> snakeCoords[2];
        int foodX;
        int foodY;
        int eaten = 0;
        int dead = 0;
        int random;

    void updateRandom();
    void moveFood();
    void init();
    int collided(int newHead[2]);
    void drawState(WINDOW* win);
    void update(int pressed);
};

WINDOW* initCurses();

#endif