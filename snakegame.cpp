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

    void drawState(WINDOW* win, int gen, float input[L1], int high) {
        attron(COLOR_PAIR(1));
        for (int y = 0; y < HEIGHT; y++) {
            mvhline(y, 0, ' ', WIDTH);
        }

        attron(COLOR_PAIR(0));
        char msg[50];

        sprintf(msg, "Gen: %d, Tick: %d", gen, ticksLived);
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