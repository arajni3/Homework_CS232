/**
 * fifteen.c
 *
 * Refer to Computer Science 50
 * Problem Set 3
 *
 * Implements Game of Fifteen (generalized to d x d).
 *
 * Usage: fifteen d
 *
 * whereby the board's dimensions are to be d x d,
 * where d must be in [DIM_MIN,DIM_MAX]
 *
 * Note that usleep is obsolete, but it offers more granularity than
 * sleep and is simpler to use than nanosleep; `man usleep` for more.
 */

#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// constants
#define DIM_MIN 3
#define DIM_MAX 9

#ifndef INT_MAX
    #define INT_MAX 12345678
#endif // INT_MAX

// board
int board[DIM_MAX][DIM_MAX];

// dimensions
int d;

// prototypes
void greet(void);
void init(void);
void draw(void);
short move(int tile);
short won(void);
int get_int();

int main(int argc, char* argv[])
{
    // ensure proper usage
    if (argc != 2)
    {
        printf("Usage: fifteen d\n");
        return 1;
    }

    // ensure valid dimensions
    d = atoi(argv[1]);
    if (d < DIM_MIN || d > DIM_MAX)
    {
        printf("Board must be between %i x %i and %i x %i, inclusive.\n",
            DIM_MIN, DIM_MIN, DIM_MAX, DIM_MAX);
        return 2;
    }

    // open log
    FILE* file = fopen("log.txt", "w");
    if (file == NULL)
    {
        return 3;
    }

    // greet user with instructions
    greet();

    // initialize the board
    init();

    // accept moves until game is won
    while (1)
    {
        // draw the current state of the board
        draw();

        // log the current state of the board (for testing)
        for (int i = 0; i < d; i++)
        {
            for (int j = 0; j < d; j++)
            {
                fprintf(file, "%i", board[i][j]);
                if (j < d - 1)
                {
                    fprintf(file, "|");
                }
            }
            fprintf(file, "\n");
        }
        fflush(file);

        // check for win
        if (won())
        {
            printf("ftw!\n");
            break;
        }

        // prompt for move
        printf("Tile to move: ");
        int tile = get_int();
        printf("\n");

        // quit if user inputs 0 (for testing)
        if (tile == 0)
        {
            break;
        }

        // log move (for testing)
        fprintf(file, "%i\n", tile);
        fflush(file);

        // move if possible, else report illegality
        if (!move(tile))
        {
            printf("\nIllegal move.\n");
            usleep(500000);
        }

        // sleep thread for animation's sake
        usleep(500000);
    }

    // close log
    fclose(file);

    // success
    return 0;
}

/**
 * Get an non-negative integer from user input
 * If the input is not non-negative integer, return INT_MAX
 */
 int get_int()
 {
    int input = 0;
    short invalid = 0;

    char c = getchar();
    if (c == '\n')
        return INT_MAX;

    while (c != '\n') 
    {
        if ((c >= '0') && ( c <= '9'))
        {
            input = input * 10 + (c - '0');
        } else {
            invalid = 1;
        }

        c = getchar();
    }

    if (invalid)
        return INT_MAX;
    else 
        return input;
 }

/**
 * Greets player.
 */
void greet(void)
{    
    printf("WELCOME TO GAME OF FIFTEEN\n");
    usleep(2000000);
}

/**
 * Initializes the game's board with tiles numbered 1 through d*d - 1
 * (i.e., fills 2D array with values but does not actually print them).
 */
void init(void)
{
    // TODO
    for (int i = 0; i < d; ++i) {
        for (int j = 0; j < d; ++j) {
            
            // empty space is denoted by a tile of 0
            if ((i == d - 1) && (j == d - 1)) {
                board[i][j] = 0;
                break;
            }

            /**
            * formula for board[i][j] when board[i][j] is not the empty space is 
            * board[i][j] = d * (d - (i + 1)) + (d - (j + 1));
            * this formula is equivalent to the one below, but the one below 
            * is more similar to the formula to check if the game is won in the function won() below
            */
            board[i][j] = (d * d) - (d * i) - (j + 1);
        }
    }
}

/**
 * Prints the board in its current state.
 */
void draw(void)
{
    // TODO
    for (int i = 0; i < d; ++i) {
        for (int j = 0; j < d; ++j) {
            if (board[i][j] == 0) {
                printf("_ ");
            } else {
                printf("%d ", board[i][j]);
            }
        }
        putchar('\n');
    }
}

/**
 * If tile borders empty space, moves tile and returns 1, else
 * returns 0.
 */
short move(int tile)
{
    // TODO

    // return 0 if tile is not valid 
    if ((tile < 1) || (tile >= d * d)) {
        return 0;
    }

    // initialize the respective coordinates of the tile and the (sole) empty space
    int tile_x = -1, tile_y = -1, empty_x = -1, empty_y = -1;

    for (int i = 0; i < d; ++i) {
        for (int j = 0; j < d; ++j) {
            if (board[i][j] == tile) {
                tile_x = i;
                tile_y = j;
            }
            if (board[i][j] == 0) {
                empty_x = i;
                empty_y = j;
            }

            // end inner loop if both pairs of coordinates have been found
            if (!(tile_x == -1 || tile_y == -1 || empty_x == -1 || empty_y == -1)) {
                break;
            }
        }

        // end outer loop if both pairs of coordinates have been found
        if (!(tile_x == -1 || tile_y == -1 || empty_x == -1 || empty_y == -1)) {
            break;
        }
    }

    // move tile into empty space by swapping the respective values of the tile and the empty space
    board[empty_x][empty_y] = board[tile_x][tile_y];
    board[tile_x][tile_y] = 0;

    return 1;
}

/**
 * Returns 1 if game is won (i.e., board is in winning configuration),
 * else 0.
 */
short won(void)
{
    // TODO

    // empty space should be denoted by a tile of 0
    if (board[d - 1][d - 1] != 0) {
        return 0;
    }

    for (int i = 0; i < d - 1; ++i) {
        for (int j = 0; j < d - 1; ++j) {
            
            // use formula for board[i][j] if game is won and board[i][j] is not supposed to be empty
            //board[i][j] = d * (d - (i + 1)) + (d - (j + 1));
            if (board[i][j] != (d * i) + (j + 1)) {
                return 0;
            }
        }
    }

    // if reached here, then game is won
    return 1;
}
