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
        int random = (int) time(0) % (WIDTH + HEIGHT);

    void updateRandom() {
        // fnv1a
        random = ((random ^ 0x811C9DC5) * 0x01000193) % 1000000;
    }

    void moveFood() {
        updateRandom();
        foodX = random % WIDTH;
        updateRandom();
        foodY = random % HEIGHT;
    }

    void init() {
        int midX = (int) WIDTH / 2;
        int midY = (int) HEIGHT / 2;

        snakeCoords[0].push_back(midX - 15);
        snakeCoords[0].push_back(midX - 14);
        snakeCoords[0].push_back(midX - 13);
        snakeCoords[0].push_back(midX - 12);

        for (int i = 0; i < 4; i++)
            snakeCoords[1].push_back(midY);

        foodX = midX + 10;
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

    void drawState(WINDOW* win) {
        attron(COLOR_PAIR(1));
        for (int y = 0; y < HEIGHT; y++) {
            mvhline(y, 0, ' ', WIDTH);
        }
        
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

    init_pair(0, COLOR_GREEN, COLOR_GREEN); // ground  
    init_pair(1, COLOR_RED, COLOR_GREEN); // food
    init_pair(2, COLOR_YELLOW, COLOR_BLUE); // snake

    return newwin(0, 0, 0, 0);
}

int main() {
    SnakeGame snake;
    snake.init();

    WINDOW* win = initCurses();

    int ch = KEY_RIGHT;
    int newCh = 0;

    while (!snake.dead) {
        snake.drawState(win);
        usleep(35000);
        newCh = getch();

        switch (newCh) {
            case KEY_UP: {
                if (ch != KEY_DOWN) {
                    ch = newCh;
                }
                break;
            } case KEY_DOWN: {
                if (ch != KEY_UP) {
                    ch = newCh;
                }
                break;
            } case KEY_LEFT: {
                if (ch != KEY_RIGHT) {
                    ch = newCh;
                }
                break;
            } case KEY_RIGHT: {
                if (ch != KEY_LEFT) {
                    ch = newCh;
                }
                break;
            }
        }

        snake.update(ch);
    }
    
    endwin();
    printf("Length -> %ld\n", snake.snakeCoords[0].size() + 1);
}