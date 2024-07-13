#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "raylib.h"

const int padding = 50;

const int windowWidth = 840;
const int windowHeight = 600;

const int squarelength = 40;

const int xcellcount = (windowWidth / squarelength);
const int ycellcount = (windowHeight / squarelength);

const int maxlength = xcellcount * ycellcount;

const float fontsize = 24.;
const float spacing = 1.;

const char *text = "Press [enter] to play.";

bool play;

typedef enum
{
    UP,
    DOWN,
    LEFT,
    RIGHT
} Direction;

typedef struct
{
    int x;
    int y;
} Index2D;

typedef struct
{
    Index2D index;
    Vector2 point;
    bool containssnake;
    bool containsfood;
} Cell;

typedef struct
{
    Cell *body[maxlength];
    Direction direction;
    int size;
    bool canMove;

} Snake;

typedef struct
{
    Cell **cells;
} Grid;

typedef struct
{
    Snake snake;
    Cell *food;
    int score;
    Grid grid;
    int framecounter;
} SnakeGame;

void init(SnakeGame *game);
void freegame(SnakeGame *game);
void step(SnakeGame *game);
void draw(SnakeGame *game);

int main(void)
{
    srand(time(NULL));
    play = false;
    SnakeGame game;

    SetTargetFPS(60);

    InitWindow(windowWidth + padding, windowHeight + padding, "raylib - Snake");

    while (!WindowShouldClose())
    {
        step(&game);
        draw(&game);
    }
    CloseWindow();

    freegame(&game);

    return 0;
}

void initgrid(Grid *grid)
{
    grid->cells = (Cell **)malloc(xcellcount * sizeof(Cell *));
    for (int i = 0; i < xcellcount; i++)
    {
        grid->cells[i] = (Cell *)malloc(ycellcount * sizeof(Cell));
        for (int j = 0; j < ycellcount; j++)
        {
            Cell cell = {(Index2D){i, j}, (Vector2){(padding / 2) + (i * squarelength) + (squarelength / 2), (padding / 2) + (j * squarelength) + (squarelength / 2)}, false, false};
            grid->cells[i][j] = cell;
        }
    }
}

void freegrid(Grid *grid)
{
    for (int i = 0; i < xcellcount; i++)
    {
        free(grid->cells[i]);
    }
    free(grid->cells);
    grid->cells = NULL;
}

void setfood(SnakeGame *game)
{
    int x, y;
    do
    {
        x = rand() % xcellcount;
        y = rand() % ycellcount;
    } while (game->grid.cells[x][y].containssnake);
    Cell *cell = &game->grid.cells[x][y];
    cell->containsfood = true;
    game->food = cell;
}

void init(SnakeGame *game)
{
    game->score = 0;
    game->framecounter = 0;
    initgrid(&game->grid);

    game->snake.body[0] = &game->grid.cells[xcellcount / 2][ycellcount / 2];
    game->snake.body[0]->containssnake = true;
    game->snake.canMove = true;

    for (int i = 1; i < maxlength; i++)
    {
        game->snake.body[i] = NULL;
    }
    game->snake.direction = RIGHT;
    game->snake.size = 1;

    setfood(game);
}

void freegame(SnakeGame *game)
{
    freegrid(&game->grid);
}

void step(SnakeGame *game)
{
    if (play)
    {
        if (game->snake.canMove)
        {
            if ((IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) && game->snake.direction != DOWN)
            {
                game->snake.direction = UP;
                game->snake.canMove = false;
            }
            else if ((IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) && game->snake.direction != UP)
            {
                game->snake.direction = DOWN;
                game->snake.canMove = false;
            }
            else if ((IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) && game->snake.direction != RIGHT)
            {
                game->snake.direction = LEFT;
                game->snake.canMove = false;
            }
            else if ((IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) && game->snake.direction != LEFT)
            {
                game->snake.direction = RIGHT;
                game->snake.canMove = false;
            }
        }

        if (game->framecounter == 5)
        {
            Cell *next;
            if (game->snake.direction == UP)
            {
                if (game->snake.body[0]->index.y - 1 < 0)
                {
                    play = false;
                    return;
                }
                next = &game->grid.cells[game->snake.body[0]->index.x][game->snake.body[0]->index.y - 1];
            }
            else if (game->snake.direction == DOWN)
            {
                if (game->snake.body[0]->index.y + 1 >= ycellcount)
                {
                    play = false;
                    return;
                }
                next = &game->grid.cells[game->snake.body[0]->index.x][game->snake.body[0]->index.y + 1];
            }
            else if (game->snake.direction == LEFT)
            {
                if (game->snake.body[0]->index.x - 1 < 0)
                {
                    play = false;
                    return;
                }
                next = &game->grid.cells[game->snake.body[0]->index.x - 1][game->snake.body[0]->index.y];
            }
            else if (game->snake.direction == RIGHT)
            {
                if (game->snake.body[0]->index.x + 1 >= xcellcount)
                {
                    play = false;
                    return;
                }
                next = &game->grid.cells[game->snake.body[0]->index.x + 1][game->snake.body[0]->index.y];
            }
            else
                exit(-1);

            if (!(next->index.x == game->snake.body[game->snake.size - 1]->index.x && next->index.y == game->snake.body[game->snake.size - 1]->index.y))
            {
                if (next->containssnake)
                {
                    play = false;
                    return;
                }
            }
            if (next->containsfood)
            {
                next->containsfood = false;
                next->containssnake = true;

                for (int i = (game->snake.size); i > 0; i--)
                {
                    game->snake.body[i] = game->snake.body[i - 1];
                }
                game->snake.size++;
                game->snake.body[0] = next;
                setfood(game);
                game->score += 10;
            }
            else
            {
                if (game->snake.size == 1)
                {
                    game->snake.body[0]->containssnake = false;
                }
                else
                {
                    game->snake.body[game->snake.size - 1]->containssnake = false;
                    for (int i = (game->snake.size - 1); i > 0; i--)
                    {
                        game->snake.body[i] = game->snake.body[i - 1];
                        game->snake.body[i]->containssnake = true;
                    }
                }
                next->containssnake = true;
                game->snake.body[0] = next;
            }
            game->framecounter = 0;
            game->snake.canMove = true;
        }
        else
            game->framecounter++;
    }
    else
    {
        if (IsKeyPressed(KEY_ENTER))
        {
            play = true;
            init(game);
        }
    }
}

void draw(SnakeGame *game)
{
    BeginDrawing();
    ClearBackground(BLACK);
    Font font = GetFontDefault();

    if (play)
    {
        for (int i = 0; i < xcellcount; i++)
        {
            for (int j = 0; j < ycellcount; j++)
            {
                Cell *cell = &game->grid.cells[i][j];
                if (cell->containssnake)
                {
                    DrawRectangle(cell->point.x - (squarelength / 2), cell->point.y - (squarelength / 2), squarelength, squarelength, GREEN);
                }
                else if (cell->containsfood)
                {
                    DrawRectangle(cell->point.x - (squarelength / 2), cell->point.y - (squarelength / 2), squarelength, squarelength, RED);
                }
                else
                {
                    DrawRectangleLines(cell->point.x - (squarelength / 2), cell->point.y - (squarelength / 2), squarelength, squarelength, GREEN);
                }
            }
        }
        char scoretext[12];
        snprintf(scoretext, sizeof(scoretext), "Score: %d", game->score);
        DrawText(scoretext, padding / 2, windowHeight + (padding / 2) * 1.1, 12, GREEN);
    }
    else
    {
        Vector2 textsize = MeasureTextEx(font, text, fontsize, spacing);
        DrawTextEx(font, text, (Vector2){(windowWidth - textsize.x + padding) / 2, (windowHeight - textsize.y + padding) / 2}, fontsize, spacing, GREEN);
    }
    EndDrawing();
}
