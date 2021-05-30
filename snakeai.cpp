#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <deque>
#include <ctime>
#include <math.h>
#include <ncurses.h>
#include <unistd.h>

#define HEIGHT 40
#define WIDTH 125

#define SNAKES 200
#define ONWARD_SNAKES 5

#define SEED 0x9206420

#define L1 6
#define L2 12
#define L3 4

using namespace std;

float sigmoid(float x) {
    return 1 / (1 + pow(2.718281828459045, x * -1));
}

class SnakeGame {
    public:
        deque<int> snakeCoords[2];
        int foodX;
        int foodY;
        int eaten = 0;
        int dead = 0;
        int ticksLived = 0;

    void moveFood() {
        foodX = rand() % WIDTH;
        foodY = rand() % HEIGHT;
    }

    void init() {
        // srand(SEED);
        int midX = (int) WIDTH / 2;
        int midY = (int) HEIGHT / 2;

        snakeCoords[0].push_back(0);//midX - 16);
        snakeCoords[0].push_back(1);
        snakeCoords[0].push_back(2);
        snakeCoords[0].push_back(3);
        snakeCoords[0].push_back(4);

        for (int i = 0; i < 5; i++)
            snakeCoords[1].push_back(midY);

        foodX = 15;
        foodY = midY;
    } 

    int collided(int newHead[2]) {
        if (newHead[0] < 0 || newHead[0] >= WIDTH || newHead[1] < 0 || newHead[1] >= HEIGHT) {
            return 1;
        }

        for (int i = 0; i < snakeCoords[0].size(); i++) {
            if (snakeCoords[0][i] == newHead[0] && snakeCoords[1][i] == newHead[1]) {
                return 1;
            }
        }
        return 0;
    }

    int collided(int x, int y) {
        int newHead[2] = { x, y };
        if (newHead[0] < 0 || newHead[0] >= WIDTH || newHead[1] < 0 || newHead[1] >= HEIGHT) {
            return 5;
        }

        for (int i = 0; i < snakeCoords[0].size(); i++) {
            if (snakeCoords[0][i] == newHead[0] && snakeCoords[1][i] == newHead[1]) {
                return 5;
            }
        }
        return -5;
    }

    int getOpenTiles(char dir) {
        switch (dir) {
            case 'l': {
                return collided(snakeCoords[0].back() - 1, snakeCoords[1].back());
            } case 'r': {
                return collided(snakeCoords[0].back() + 1, snakeCoords[1].back());
            } case 'd': {
                return collided(snakeCoords[0].back(), snakeCoords[1].back() + 1);
            } case 'u': {
                return collided(snakeCoords[0].back(), snakeCoords[1].back() - 1);
            }
        }
        return -1;
    }

    void drawState(WINDOW* win, int gen, int i, float input[L1], int high) {
        attron(COLOR_PAIR(1));
        for (int y = 0; y < HEIGHT; y++) {
            mvhline(y, 0, ' ', WIDTH);
        }

        attron(COLOR_PAIR(0));
        char msg[50];

        sprintf(msg, "Gen: %d, Snake: %d, Tick: %d", gen, i, ticksLived);
        mvaddstr(0, 0, msg);

        char inputs[300];
        sprintf(inputs, "fX: %f, fY: %f, l: %f, r: %f, u: %f, d: %f, HIGH: %d", 
                input[0], input[1], input[2], input[3], input[4], input[5], high);

        mvaddstr(1, 0, inputs);
        
        mvaddch(foodY, foodX, '@');

        attron(COLOR_PAIR(2));
        for (int i = 0; i < snakeCoords[0].size(); i++) {
            if (i == snakeCoords[0].size() - 1) {
                mvaddch(snakeCoords[1][i], snakeCoords[0][i], ':');
            } else {
                mvaddch(snakeCoords[1][i], snakeCoords[0][i], ' ');
            }
        }

        refresh();
    }

    void update(int pressed) {
        if (dead) {
            return;
        }

        if (eaten == 0) {
            snakeCoords[0].pop_front();
            snakeCoords[1].pop_front();
        } else {
            --eaten;
        }

        int newHead[2] = { snakeCoords[0].back(), snakeCoords[1].back() };

        switch (pressed) {
            case KEY_LEFT: {
                newHead[0] -= 1;
                break;
            } case KEY_RIGHT: {
                newHead[0] += 1;
                break;
            } case KEY_UP: {
                newHead[1] -= 1;
                break;
            } case KEY_DOWN: {
                newHead[1] += 1;
                break;
            }
        }

        if (collided(newHead)) {
            dead = 1;
            return;
        }

        if (newHead[0] == foodX && newHead[1] == foodY) {
            eaten += 3;
            moveFood();
        }

        snakeCoords[0].push_back(newHead[0]);
        snakeCoords[1].push_back(newHead[1]);

        ticksLived++;
    }
};

class NeuralNet {
    private:

    float layer2[L2];
    float layer3[L3];
    float weights1[L1 * L2];
    float weights2[L2 * L3];
    float bias1[L2];
    float bias2[L3];

    int max;
    float t;
    float e = 2.718281828459045;

    float getRand() {
        float f = (float) rand() / 1073741823.5;
        return f - 1;
    }

    float biasedSigmoid(float x, float sigmoidBias) {
        return 1 / (1 + pow(e, (x + sigmoidBias) * -1));
    }

    public:

    void init() {
        for (int i = 0; i < L1 * L2; i++) {
            if (i < L2 * L3)
                weights2[i] = getRand(); 
            if (i < L2)
                bias1[i] = getRand();
            if (i < L3)
                bias2[i] = getRand();
            weights1[i] = getRand(); 
        }
    }

    void mutate(int mutationLevel) {
        for (int i = 0; i < L1 * L2; i++) {
            weights1[i] += getRand() / (float) mutationLevel;
        }

        for (int i = 0; i < L2 * L3; i++) {
            weights2[i] += getRand() / (float) mutationLevel;
        }

        for (int i = 0; i < L2; i++) {
            bias1[i] = getRand() / (float) mutationLevel;
        }

        for (int i = 0; i < L3; i++) {
            bias2[i] = getRand() / (float) mutationLevel;
        }
    }

    float *feedForward(float inputs[L1]) {
        for (int i = 0; i < L2; i++) {
            t = 0;
            for (int j = 0; j < L1; j++) {
                t += weights1[(i + 1) * j] * inputs[j];
            }
            layer2[i] = biasedSigmoid(t, bias1[i]);
        }
        
        for (int i = 0; i < L3; i++) {
            t = 0;
            for (int j = 0; j < L2; j++) {
                t += weights2[(i + 1) * j] * layer2[j];
            }
            layer3[i] = biasedSigmoid(t, bias2[i]);
        }

        return layer3;
    }
};

WINDOW* initCurses() {
    initscr();
    keypad(stdscr, true);
    nonl(); 
    cbreak();
    noecho();
    timeout(1);
    curs_set(0);

    start_color();

    init_pair(0, COLOR_WHITE, COLOR_BLACK); // text
    init_pair(1, COLOR_RED, COLOR_GREEN); // food
    init_pair(2, COLOR_YELLOW, COLOR_BLUE); // snake

    return newwin(0, 0, 0, 0);
}

int main() {
    int i;
    float input[L1];
    float *output;
    int high = 0;

    NeuralNet snakes[SNAKES];
    int scores[SNAKES];
    int highScores[ONWARD_SNAKES] = { 0 };

    WINDOW* win = initCurses();

    srand(time(0));

    for (i = 0; i < SNAKES; i++) {
        snakes[i].init();
    }

    int gen = 0;
    int lastCh = -1;

    while (1) {
        srand(time(0));
        for (i = 0; i < SNAKES; i++) {
            SnakeGame sg;
            sg.init();

            lastCh = 1;

            while (!sg.dead && sg.ticksLived < 500) {
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

                output = snakes[i].feedForward(input);
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
                    }
                }
                
                if ((sg.snakeCoords[0].size() >= high && gen != 0) || (i == 0 && gen % 5 == 0)) {
                    sg.drawState(win, gen, i, input, high);
                    usleep(12500);
                }
            }

            if (gen < 50) {
                scores[i] = sg.snakeCoords[0].size() * 3000 + sg.ticksLived + (WIDTH - abs(sg.snakeCoords[0].back() - sg.foodX)) + (HEIGHT - abs(sg.snakeCoords[1].back() - sg.foodY));
            } else {
                scores[i] = sg.snakeCoords[0].size() * 3000 + sg.ticksLived;
            }

            if (sg.snakeCoords[0].size() > high) {
                high = sg.snakeCoords[0].size();
            }
        }

        for (i = 0; i < SNAKES; i++) {
            int x = rand() % ONWARD_SNAKES; 
            if (scores[i] > scores[highScores[x]]) {
                highScores[x] = i;
            }
        }

        for (i = 0; i < SNAKES; i++) {
            snakes[i] = snakes[highScores[rand() % ONWARD_SNAKES]];
            if (i >= ONWARD_SNAKES) {
                snakes[i].mutate(1 + rand() % 2000 + gen / 50);
            }
        }

        gen++;
    }

    endwin();
}