#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <GL/gl.h>
#include <GL/glut.h>  // freeglut.h might be a better alternative, if available.
#include <sys/stat.h>

#include "queue.h"
#include "time.h"
#include "shape.h"
#include "array.h"
#include "helpers.h"


#define MS_PER_FRAME 16  // 16 milliseconds per frame for ~60 FPS
#define TICK_RATE 2

#define BOARD_START_X 200
#define BOARD_START_Y 100
#define BOARD_WIDTH 400
#define BOARD_HEIGHT 800

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 1000

#define BOARD_COLS 10
#define BOARD_ROWS 20

#define PX_PER_WIDTH (BOARD_WIDTH / BOARD_COLS)
#define PX_PER_HEIGHT (BOARD_HEIGHT / BOARD_ROWS)

typedef struct
{
    double tick_rate;
    bool started;
    bool paused;
    bool gameover;
    int board[BOARD_ROWS][BOARD_COLS];
    Shape shape;
    int shapes[6][4][4];
} GameState;

GameState state;


int getTetrisRow()
{
    for (int i = 0; i < BOARD_ROWS; i++)
    {
        int colCount = 0;
        for (int j = 0; j < BOARD_COLS; j++)
        {
            if (state.board[i][j] == 1)
            {
                colCount++;
            }
        }

        if (colCount == BOARD_COLS)
        {
            return i;
        }
    }

    return 0;
}

void selectRandomShape()
{
    int x = random(0, 5);

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            state.shape.body[i][j] = state.shapes[x][i][j];
        }
    }
}

void collapseRow(int index)
{
    for (int i = index; i > 0; i--)
    {
        for (int j = 0; j < BOARD_COLS; j++)
        {
            state.board[i][j] = state.board[i - 1][j];
        }
    }

    state.tick_rate -=50;
}

int getShapeSize()
{
    for (int i = 0; i < 4; i++)
    {
        if(state.shape.body[i][3] == 1 || state.shape.body[3][i] == 1)
        {
            return 4;
        }
    }


    return 3;
}

void getNextRotation(int shape[4][4])
{
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
           shape[i][j] = state.shape.body[i][j];
        }
    }

    int size = getShapeSize();

    transpose(shape, size);
    reverseRows(shape, size);
}

void rotate()
{
    int size = getShapeSize();

    transpose(state.shape.body, size);
    reverseRows(state.shape.body, size);
}

bool checkCollision(int dx, int dy, bool checkNextRotation)
{
    int shape[4][4];

    getNextRotation(shape);

    for (int row = 0; row < 4; row++)
    {
        for (int col = 0; col < 4; col++)
        {
            if(checkNextRotation)
            {
                if (shape[row][col] == 1)
                {
                    int x = col + state.shape.x + dx;
                    int y = row + state.shape.y + dy;
                    if (y >= BOARD_ROWS || y <= 0 || x >= BOARD_COLS || x < 0)
                    {
                        return true;
                    }
                }
            } else
            {
                if (state.shape.body[row][col] == 1)
                {
                    int x = col + state.shape.x + dx;
                    int y = row + state.shape.y + dy;
                    if (y >= BOARD_ROWS || y <= 0 || x >= BOARD_COLS || x < 0)
                    {
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

bool checkGroundedCollision(int dx, int dy)
{
    for (int row = 0; row < 4; row++)
    {
        for (int col = 0; col < 4; col++)
        {
            if (state.shape.body[row][col] == 1)
            {
                int x = col + state.shape.x;
                int y = row + state.shape.y;

                printf("x = %d, y = %d\n", x, y);

                if (y == BOARD_ROWS - 1 || state.board[y + dy][x + dx] == 1)
                {
                    return true;
                }
            }
        }
    }

    return false;
}

void newShape()
{
    state.shape.x = 4;
    state.shape.y = 0;

    selectRandomShape();
}

void applyShapeToBoard()
{
    for (int row = 0; row < 4; row++)
    {
        for (int col = 0; col < 4; col++)
        {
            if (state.shape.body[row][col] == 1)
            {
                int x = col + state.shape.x;
                int y = row + state.shape.y;

                state.board[y][x] = 1;
            }
        }
    }
}

bool isGameover()
{
    return state.shape.y <= 4 && checkGroundedCollision(0, 1);

}


void updateGame()
{
    if (!state.started)
    {
        return;
    }

    if(state.gameover)
    {
        return;
    }

    if(isGameover())
    {
        state.gameover = true;
    }

    if (!checkCollision(0, 1, state.shape.rotationIndex))
    {
        state.shape.y++;
    }

    if (checkGroundedCollision(0, 1))
    {
        applyShapeToBoard();

        newShape();
    }

    int row = getTetrisRow();

    if (row != 0)
    {
        collapseRow(row);
    }
}

void renderWalls()
{
    glBegin(GL_LINES);
    glColor3f(1.0, 1.0, 1.0);

    glVertex2d(BOARD_START_X, BOARD_START_Y);
    glVertex2d(BOARD_START_X + BOARD_WIDTH, BOARD_START_Y);

    glVertex2d(BOARD_START_X + BOARD_WIDTH, BOARD_START_Y);
    glVertex2d(BOARD_START_X + BOARD_WIDTH, BOARD_START_Y + BOARD_HEIGHT);

    glVertex2d(BOARD_START_X + BOARD_WIDTH, BOARD_START_Y + BOARD_HEIGHT);
    glVertex2d(BOARD_START_X, BOARD_START_Y + BOARD_HEIGHT);

    glVertex2d(BOARD_START_X, BOARD_START_Y + BOARD_HEIGHT);
    glVertex2d(BOARD_START_X, BOARD_START_Y);
    glEnd();
}

void renderBoard()
{
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_QUADS);
    for (int row = 0; row < BOARD_ROWS; row++)
    {
        for (int col = 0; col < BOARD_COLS; col++)
        {
            if (state.board[row][col] == 1)
            {
                int startX = BOARD_START_X + PX_PER_WIDTH * col;
                int startY = BOARD_START_Y + PX_PER_HEIGHT * row;

                glVertex2d(startX, startY);
                glVertex2d(startX + PX_PER_WIDTH, startY);
                glVertex2d(startX + PX_PER_WIDTH, startY + PX_PER_HEIGHT);
                glVertex2d(startX, startY + PX_PER_HEIGHT);
            }
        }
    }

    glEnd();
}

void renderShape()
{
    glBegin(GL_QUADS);
    glColor3f(1.0, 1.0, 1.0);

    for (int row = 0; row < 4; row++)
    {
        for (int col = 0; col < 4; col++)
        {
            if (state.shape.body[row][col] == 1)
            {
                int startX = BOARD_START_X + PX_PER_WIDTH * (col + state.shape.x);
                int startY = BOARD_START_Y + PX_PER_HEIGHT * (row + state.shape.y);

                glVertex2d(startX, startY);
                glVertex2d(startX + PX_PER_WIDTH, startY);
                glVertex2d(startX + PX_PER_WIDTH, startY + PX_PER_HEIGHT);
                glVertex2d(startX, startY + PX_PER_HEIGHT);
            }
        }
    }
    glEnd();
}


void render()
{
    // This function is called to render graphics
    glClear(GL_COLOR_BUFFER_BIT);

    renderBitmapString(10, 50, GLUT_BITMAP_HELVETICA_12, "SPACE to start");
    renderBitmapString(10, 70, GLUT_BITMAP_HELVETICA_12, "R to restart");

    if(state.gameover)
    {
        renderBitmapString(500, 50, GLUT_BITMAP_HELVETICA_18, "GAME OVER");
    }

    renderWalls();
    renderBoard();
    renderShape();
    glutSwapBuffers(); // Swap the front and back buffers
}

void display()
{
    // Call render to draw the frame
    render();
}

void timer(int value)
{
    static double previous = 0.0;
    static double lag = 0.0;
    static double tickLag = 0.0;

    double current = getCurrentTime();
    double elapsed = current - previous;
    previous = current;
    lag += elapsed;
    tickLag += elapsed;

    while (tickLag >= state.tick_rate)
    {
        updateGame();
        tickLag -= state.tick_rate;
    }

    display(); // Call the display function to render the updated frame

    // Call the timer function again after the defined interval
    glutTimerFunc(MS_PER_FRAME, timer, 0); // 16 ms for ~60 FPS
}

void initGameState()
{
    for (int i = 0; i < BOARD_ROWS; i++)
    {
        for (int j = 0; j < BOARD_COLS; j++)
        {
            state.board[i][j] = 0;
        }
    }

    state.tick_rate = 1000 / TICK_RATE;

    int shapes[6][4][4] = {
        {
            {1, 1, 1, 0,},
            {0, 0, 1, 0,},
            {0, 0, 0, 0,},
            {0, 0, 0, 0,}
        },
        {
            {0, 0, 0, 0,},
            {0, 0, 0, 0,},
            {1, 1, 1, 1},
            {0, 0, 0, 0,}
        },
        {
            {0, 0, 0, 0,},
            {1, 1, 1, 0,},
            {1, 0, 0, 0,},
            {0, 0, 0, 0,}
        },
        {
            {0, 0, 0, 0,},
            {0, 1, 1, 0,},
            {1, 1, 0, 0,},
            {0, 0 ,0, 0,}
        },
        {
            {0, 0, 0, 0,},
            {1, 1, 1, 0,},
            {0, 1, 0, 0,},
            {0, 0, 0, 0,}
        },
        {
            {0, 0, 0, 0,},
            {1, 1, 0, 0,},
            {0, 1, 1, 0,},
            {0, 0, 0, 0,}
        },
    };


    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            for (int k = 0; k < 4; k++)
            {
                state.shapes[i][j][k] = shapes[i][j][k];
            }
        }
    }

    selectRandomShape();

    state.gameover = false;
    state.shape.x = 4;
    state.shape.y = 0;
    state.shape.rotation = (int (*)[4][4])malloc(sizeof(int) * 4 * 4 * 4);


}

void restart()
{
    initGameState();
}

void levelUp()
{
    state.tick_rate -= 3;
}

void pause()
{
    state.paused = !state.paused;
}

void start()
{
    state.started = true;
}

void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'w': // Move up
        if(!state.started || state.gameover)
        {
            return;
        }

        if (checkCollision(0, 0, true))
        {
            return;
        }
        rotate();
        break;
    case 's': // Move down
        if(!state.started || state.gameover)
        {
            return;
        }

        if (checkCollision(0, 1, false))
        {
            return;
        }

        if (checkGroundedCollision(0, 1))
        {
            applyShapeToBoard();

            newShape();
        }

        state.shape.y++;
        break;
    case 'a': // Move left
        if(!state.started || state.gameover)
        {
            return;
        }

        if (checkCollision(-1, 0, false))
        {
            return;
        }

        if (checkGroundedCollision(-1, 0))
        {
            applyShapeToBoard();

            newShape();
        }

        state.shape.x--;
        break;
    case 'd': // Move right
        if(!state.started || state.gameover)
        {
            return;
        }

        if (checkCollision(1, 0, false))
        {
            return;
        }
        state.shape.x++;

        if (checkGroundedCollision(1, 0))
        {
            applyShapeToBoard();

            newShape();
        }
        break;
    case 'x':
        // levelUp();
        break;
    case 'p':
        pause();
        break;
    case 'r':
        restart();
        break;
    case 32:
        if (!state.started)
        {
            start();
        }
        break;
    case 27: // ESC key to exit
        exit(0);
        break;
    }
    glutPostRedisplay();
}


int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB); // Use double buffering
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT); // Size of display area
    glutInitWindowPosition(400, 100); // Location of window
    glutCreateWindow("GL RGB Triangle"); // Window title
    initGameState(&state);
    glMatrixMode(GL_PROJECTION); // Set the projection matrix mode
    glLoadIdentity(); // Load the identity matrix
    glOrtho(0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, -1, 1);

    // Register the display callback
    glutDisplayFunc(display);

    // Start the timer
    glutTimerFunc(0, timer, 0); // Start the timer immediately
    glutKeyboardFunc(keyboard);

    glutMainLoop(); // Run the event loop

    return 0;
}
