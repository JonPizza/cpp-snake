#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <deque>
#include <ctime>
#include <math.h>
#include <ncurses.h>
#include <unistd.h>

#define HEIGHT 58
#define WIDTH 238

#define SNAKES 1000
#define ONWARD_SNAKES 10

#define SEED 0x6942069

#define L1 8
#define L2 16
#define L3 4

using namespace std;

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
        srand(SEED);
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
        if (newHead[0] < 0 || newHead[0] > WIDTH || newHead[1] < 0 || newHead[1] > HEIGHT) {
            return 1;
        }

        for (int i = 0; i < snakeCoords[0].size(); i++) {
            if (snakeCoords[0][i] == newHead[0] && snakeCoords[1][i] == newHead[1]) {
                return 1;
            }
        }
        return 0;
    }

    int tileOccupied(int x, int y) {
        for (int i = 0; i < snakeCoords[0].size(); i++) {
            for (int j = 0; i < snakeCoords[1].size(); i++) {
                if (snakeCoords[0][i] == x && snakeCoords[1][j] == y) {
                    return 1;
                }
            }
        }
        return 0;
    }

    int getOpenTiles(char dir) {
        switch (dir) {
            case 'l': {
                for (int i = snakeCoords[0].back(); i >= 0; i--) {
                    if (tileOccupied(i, snakeCoords[0].back()) || i == 0) {
                        return snakeCoords[0].back() - i;
                    }
                }
                break;
            } case 'r': {
                for (int i = snakeCoords[0].back(); i <= WIDTH; i++) {
                    if (tileOccupied(i, snakeCoords[0].back()) || i == WIDTH) {
                        return i - snakeCoords[0].back();
                    }
                }
                break;
            } case 'd': {
                for (int i = snakeCoords[1].back(); i <= HEIGHT; i++) {
                    if (tileOccupied(i, snakeCoords[0].back()) || i == HEIGHT) {
                        return i - snakeCoords[0].back();
                    }
                }
                break;
            } case 'u': {
                for (int i = snakeCoords[1].back(); i >= 0; i--) {
                    if (tileOccupied(i, snakeCoords[0].back()) || i == 0) {
                        return snakeCoords[1].back() - i;
                    }
                }
                break;
            }
        }
        return -1;
    }

    void drawState(WINDOW* win, int gen, int i) {
        attron(COLOR_PAIR(1));
        for (int y = 0; y < HEIGHT; y++) {
            mvhline(y, 0, ' ', WIDTH);
        }

        attron(COLOR_PAIR(0));
        char msg[50];
        sprintf(msg, "Gen: %d, Snake: %d, Tick: %d", gen, i, ticksLived);
        mvaddstr(0, 0, msg);
        
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

        if (newHead[0] == foodX && newHead[1] == foodY) {
            eaten += 3;
            moveFood();
        }

        if (collided(newHead)) {
            dead = 1;
            return;
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
    float sigmoidBias;

    int max;
    float t;
    float e = 2.718281828459045;

    float getRand() {
        float f = (float) rand() / 1073741823.5;
        return f - 1;
    }

    float sigmoid(float x) {
        return 1 / (1 + pow(e, x * -1));
    }

    float biasedSigmoid(float x) {
        return 1 / (1 + sigmoidBias + pow(e, x * -1));
    }

    public:

    void init() {
        for (int i = 0; i < L1 * L2; i++) {
            if (i < L2 * L3)
                weights2[i] = getRand(); 
            weights1[i] = getRand(); 
        }

        sigmoidBias = getRand() / 10.0;
    }

    void mutate(int mutationLevel) {
        for (int i = 0; i < L1 * L2; i++) {
            weights1[i] += getRand() / (float) mutationLevel;
        }

        for (int i = 0; i < L2 * L3; i++) {
            weights2[i] += getRand() / (float) mutationLevel;
        }

        sigmoidBias += getRand() / (float) mutationLevel / 10.0;
    }

    int feedForward(float inputs[L1]) {
        for (int i = 0; i < L2; i++) {
            t = 0;
            for (int j = 0; j < L1; j++) {
                t += weights1[(i + 1) * j] * inputs[j];
            }
            layer2[i] = biasedSigmoid(t);
        }

        for (int i = 0; i < L3; i++) {
            t = 0;
            for (int j = 0; j < L2; j++) {
                t += weights2[(i + 1) * j] * layer2[j];
            }
            layer3[i] = biasedSigmoid(t);
        }

        max = 0;
        for (int i = 0; i < L3; i++) {
            if (layer3[i] > layer3[max]) {
                max = i;
            }
        }

        return max;
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
    float input[8];
    int output;

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
        for (i = 0; i < SNAKES; i++) {
            SnakeGame sg;
            sg.init();

            lastCh = 1;

            while (!sg.dead && sg.ticksLived < 2000) {
                input[0] = sg.foodX;
                input[1] = sg.foodY;
                input[2] = sg.snakeCoords[0].back();
                input[3] = sg.snakeCoords[1].back();
                input[4] = sg.getOpenTiles('l');
                input[5] = sg.getOpenTiles('r');
                input[6] = sg.getOpenTiles('u');
                input[7] = sg.getOpenTiles('d');

                output = snakes[i].feedForward(input);

                switch (output) {
                    case 0: {
                        if (lastCh != 1) {
                            sg.update(KEY_LEFT);
                            lastCh = output;
                        } else {
                            sg.update(KEY_RIGHT);
                        }
                        break;
                    } case 1: {
                        if (lastCh != 0) {
                            sg.update(KEY_RIGHT);
                            lastCh = output;
                        } else {
                            sg.update(KEY_LEFT);
                        }
                        break;
                    } case 2: {
                        if (lastCh != 3) {
                            sg.update(KEY_UP);
                            lastCh = output;
                        } else {
                            sg.update(KEY_DOWN);
                        }
                        break;
                    } case 3: {
                        if (lastCh != 2) {
                            sg.update(KEY_DOWN);
                            lastCh = output;
                        } else {
                            sg.update(KEY_UP);
                        }
                        break;
                    }
                }
                
                if (i % 149 == 0) {
                    sg.drawState(win, gen, i);
                    usleep(5500);
                }
            }

            scores[i] = sg.snakeCoords[0].size() * 700 + sg.ticksLived + (WIDTH - abs(sg.snakeCoords[0].back() - sg.foodX)) + (HEIGHT - abs(sg.snakeCoords[1].back() - sg.foodY));
        }

        for (i = 0; i < SNAKES; i++) {
            int x = rand() % ONWARD_SNAKES; 
            if (scores[i] > scores[highScores[x]]) {
                highScores[x] = i;
            }
        }

        for (i = 0; i < SNAKES; i++) {
            snakes[i] = snakes[highScores[rand() % ONWARD_SNAKES]];
            if (i != 0) {
                snakes[i].mutate(rand() % 100 + 1);
            }
        }

        gen++;
    }

    endwin();
}