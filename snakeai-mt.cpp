#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <deque>
#include <ctime>
#include <math.h>
#include <ncurses.h>
#include <unistd.h>
#include <thread>
#include <mutex>

#define HEIGHT 40
#define WIDTH 125

#define SNAKES 1000
#define ONWARD_SNAKES 5

// (SNAKES / THREADS) must be a whole number
#define THREADS 30

#define L1 6
#define L2 14
#define L3 4

using namespace std;

#include "neuralnet.cpp"
#include "snakegame.cpp"

void playSnake(int* score, int *high, NeuralNet snake, int gen, WINDOW* win, bool canDisplay) {
    SnakeGame sg;
    sg.init();
    int lastCh = -1;
    float input[L1];
    float *output;

    while (!sg.dead && sg.ticksLived < 50 * sg.snakeCoords[0].size() && sg.ticksLived < 5000) {
        input[0] = sg.snakeCoords[0].back() - sg.foodX;
        if (input[0] < 0) {
            input[0] -= 10;
        } else if (input[0] > 0) {
            input[0] += 10;
        }

        input[1] = sg.snakeCoords[1].back() - sg.foodY;
        if (input[1] < 0) {
            input[1] -= 10;
        } else if (input[1] > 0) {
            input[1] += 10;
        }

        input[2] = sg.getOpenTiles('l');
        input[3] = sg.getOpenTiles('r');
        input[4] = sg.getOpenTiles('u');
        input[5] = sg.getOpenTiles('d');

        output = snake.feedForward(input);
        int best = -1;

        for (int i = 0; i < L3; i++) {
            if ((best == -1 || output[i] >= output[best]) && input[i + 2] <= 0) {
                best = i;
            }
        }

        switch (best) {
            case 0: {
                if (lastCh != 1) {
                    sg.update(KEY_LEFT);
                    lastCh = best;
                } else {
                    sg.update(KEY_RIGHT);
                }
                break;
            } case 1: {
                if (lastCh != 0) {
                    sg.update(KEY_RIGHT);
                    lastCh = best;
                } else {
                    sg.update(KEY_LEFT);
                }
                break;
            } case 2: {
                if (lastCh != 3) {
                    sg.update(KEY_UP);
                    lastCh = best;
                } else {
                    sg.update(KEY_DOWN);
                }
                break;
            } case 3: {
                if (lastCh != 2) {
                    sg.update(KEY_DOWN);
                    lastCh = best;
                } else {
                    sg.update(KEY_UP);
                }
                break;
            } case -1: {
                sg.update(KEY_DOWN);
                break;
            }
        }
        
        if (canDisplay && gen % 3 == 0) {
            sg.drawState(win, gen, input, *high);
            usleep(15500);
            if (*high < sg.snakeCoords[0].size()) {
                *high = sg.snakeCoords[0].size();
            }
        }
    }

    if (gen < 50) {
        *score = sg.snakeCoords[0].size() * 3000 + sg.ticksLived + (WIDTH - abs(sg.snakeCoords[0].back() - sg.foodX)) + (HEIGHT - abs(sg.snakeCoords[1].back() - sg.foodY));
    } else {
        *score = sg.snakeCoords[0].size() * 3000 + sg.ticksLived;
    }
}

int main() {
    int i, j;
    int high = 0;

    NeuralNet snakes[SNAKES];
    SnakeGame sg[THREADS];
    thread threads[THREADS];

    int scores[SNAKES];
    int highScores[ONWARD_SNAKES] = { 0 };

    WINDOW* win = initCurses();

    srand(time(0));

    for (i = 0; i < SNAKES; i++) {
        snakes[i].init();
    }

    int gen = 0;

    while (true) {
        // play snake (multithreaded)
        for (i = 0; i < (SNAKES / THREADS); i++) {
            char msg[32];
            sprintf(msg, "Snake: %d / %d", i * THREADS + (SNAKES * gen), SNAKES * (gen + 1));
            attrset(COLOR_PAIR(0));
            mvaddstr(HEIGHT, 0, msg);
            refresh();

            for (j = 0; j < THREADS; j++) {
                threads[j] = thread(playSnake, &scores[(i * THREADS) + j], &high, snakes[(i * THREADS) + j], gen, win, i + j == 0);
            }

            for (j = 0; j < THREADS; j++) {
                threads[j].join();
            }
        }

        // organize snakes based on score
        for (i = 0; i < SNAKES; i++) {
            int x = rand() % ONWARD_SNAKES; 
            if (scores[i] > scores[highScores[i % ONWARD_SNAKES]]) {
                highScores[x] = i;
            }

            if (scores[i] > scores[highScores[0]]) {
                highScores[0] = i;
            }
        }

        // mutate snakes
        snakes[0] = snakes[highScores[0]];
        for (i = 1; i < SNAKES; i++) {
            snakes[i] = snakes[highScores[rand() % ONWARD_SNAKES]];
            if (i > ONWARD_SNAKES) {
                snakes[i].mutate(1 + rand() % 300 + gen / 50);    
            }
        }

        gen++;
    }

    endwin();
}