// a simple snake game which uses pthread.h library to handle user input
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <termios.h> // for getch
#include <errno.h>
#include <unistd.h>
#include <pthread.h>

#define SIZE_X 20
#define SIZE_Y 20
#define TIME 200 // game update interval (milliseconds)

bool collision;

typedef struct position
{
    int x;
    int y;
} position;

typedef struct node
{
    position body;
    struct node *next;
} node_t;

node_t *create_new_node(position body)
{
    node_t *result = malloc(sizeof(node_t));
    result->body = body;
    result->next = NULL;
    return result;
}

void *insert_at_head(node_t **head, node_t *node_to_insert)
{
    node_to_insert->next = *head;
    *head = node_to_insert;
}

int msleep(long msec) // sleep for the requested number of milliseconds
{
    struct timespec ts;
    int res;

    if (msec < 0)
    {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do
    {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
}

char getch(void) // get char
{
    char buf = 0;
    struct termios old = {0};
    fflush(stdout);
    if (tcgetattr(0, &old) < 0)
        perror("tcsetattr()");
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if (tcsetattr(0, TCSANOW, &old) < 0)
        perror("tcsetattr ICANON");
    if (read(0, &buf, 1) < 0)
        perror("read()");
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if (tcsetattr(0, TCSADRAIN, &old) < 0)
        perror("tcsetattr ~ICANON");
    printf("%c\n", buf);
    return buf;
}

int randomInt(int b, int a) // generates random integer between b and a
{
    return ((rand() % (b + 1 - a)) + a);
}

void *readMove(void *data) // argument to p_thread to read user input
{
    int *direction = ((int *)data); // pass function void pointer to direction pointer

    pthread_detach(pthread_self());
    while (collision)
    {
        if (getch() == '\033')
        { // if the first value is esc
            fputs("\033[A\033[2K", stdout);
            rewind(stdout);
            getch(); // skip the [
            fputs("\033[A\033[2K", stdout);
            rewind(stdout);
            switch (getch())
            { // the real value
            case 'A':
                // code for arrow up
                fputs("\033[A\033[2K", stdout);
                rewind(stdout);
                *direction = 0;
                break;
            case 'B':
                // code for arrow down
                fputs("\033[A\033[2K", stdout);
                rewind(stdout);
                *direction = 2;
                break;
            case 'C':
                // code for arrow right
                fputs("\033[A\033[2K", stdout);
                rewind(stdout);
                *direction = 1;
                break;
            case 'D':
                // code for arrow left
                fputs("\033[A\033[2K", stdout);
                rewind(stdout);
                *direction = 3;
                break;
            }
        }
    }
    pthread_exit(NULL);
}

void beginGame() // welcome screen
{
    int i;
    for (i = 0; i < 4; i++)
    {
        printf("WELCOME TO C-SNAKE! - Initializing game in %d seconds ...\n", (3 - i));
        sleep(1);
        fputs("\033[A\033[2K", stdout);
        rewind(stdout);
    }
}

void clearScreen() // clear game screen
{
    int i;
    for (i = 0; i <= SIZE_Y; i++)
    {
        fputs("\033[A\033[2K", stdout);
        rewind(stdout);
    }
}

void printGame(char game[SIZE_X][SIZE_Y]) // print current game
{
    int i, j;
    printf("\r");
    for (i = 0; i < SIZE_X; i++)
    {
        printf("\n");
        for (j = 0; j < SIZE_Y; j++)
        {
            printf("%c ", game[i][j]);
        }
    }
    printf("\n");
}

void initializeGame(char game[SIZE_X][SIZE_Y], position *player_position, int *direction) // initializes variables
{
    int i, j;

    // initializes movement direction - 0: up, 1: right, 2: down, 3: left
    *direction = 2;

    // initializes player position
    player_position->x = 1;
    player_position->y = 1;

    // initializes board
    for (i = 0; i < SIZE_X; i++)
    {
        for (j = 0; j < SIZE_Y; j++)
        {
            if ((i == player_position->x) && (j == player_position->y))
            {
                game[i][j] = 'o';
            }
            else
            {
                game[i][j] = '_';
            }
        }
    }
}

bool checkCollision(position *player_position) // checks for collision (game over)
{
    if (player_position->x == 0)
    {
        return false;
    }
    else if (player_position->y == 0)
    {
        return false;
    }
    else if (player_position->x == (SIZE_X - 1))
    {
        return false;
    }
    else if (player_position->y == (SIZE_Y - 1))
    {
        return false;
    }
    else
    {
        return true;
    }
}

void updateGame(char game[SIZE_X][SIZE_Y], position *player_position, int *direction, int *length) // updates snake position
{
    int i, j, x_inc = 0, y_inc = 0;

    switch (*direction)
    {
    case 0:
        x_inc = -1;
        break;
    case 1:
        y_inc = 1;
        break;
    case 2:
        x_inc = 1;
        break;
    case 3:
        y_inc = -1;
        break;
    }

    player_position->x = player_position->x + x_inc;
    player_position->y = player_position->y + y_inc;

    for (i = 0; i < SIZE_X; i++)
    {
        for (j = 0; j < SIZE_Y; j++)
        {
            if ((i == player_position->x) && (j == player_position->y))
            {
                if (game[i][j] == '+')
                {
                    *length = *length + 1;
                }
                game[i][j] = 'o';
            }
            else if (game[i][j] != '+')
            {
                game[i][j] = '_';
            }
        }
    }
}

void generateFruit(char game[SIZE_X][SIZE_Y]) // generate random fruits
{
    position fruit_position;

    while (true)
    {
        fruit_position.x = randomInt(SIZE_X - 2, 1);
        fruit_position.y = randomInt(SIZE_Y - 2, 1);
        if (game[fruit_position.x][fruit_position.y] == '_')
        {
            game[fruit_position.x][fruit_position.y] = '+';
            break;
        }
    }
}

int main(int argc, char *argv[])
{
    char game[SIZE_X][SIZE_Y]; // game tiles
    int i, j;                  // auxiliary variables
    int length = 0;            // initial length (score)
    int counter = 0;           // time counter
    float total_time;          // total game time
    int fruit_interval = 10;   // time interval (seconds) to random fruit generation
    position player_position;  // player head position (x,y)
    int direction;             // moving direction
    int rc;
    pthread_t pthread; // thread to handle player input

    srand(time(NULL)); // uses time to generate random seed

    beginGame();                                        // intro screen
    initializeGame(game, &player_position, &direction); // initializes variables
    collision = checkCollision(&player_position);

    rc = pthread_create(&pthread, NULL, readMove, (void *)&direction);
    if (rc)
    {
        printf("\nError - return code from pthread_create is %d\n", rc);
        return EXIT_FAILURE;
    }

    while (collision)
    {
        collision = checkCollision(&player_position);
        printGame(game);
        msleep(TIME);
        clearScreen();
        updateGame(game, &player_position, &direction, &length);
        counter++;
        if ((counter % fruit_interval) == 0)
        {
            generateFruit(game);
        }
    }
    total_time = TIME * counter / 1000;
    printf("\n*** GAME OVER! ***\n\n");
    printf("\nTotal time: %.1fs\n\n", total_time);
    printf("\nTotal score: %d\n\n", length);
    puts("Press any key to quit ...");

    pthread_exit(NULL);
}
