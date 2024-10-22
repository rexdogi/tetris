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
} GameState;

GameState state;

int getTetrisRow()
{
    for (int i = 0; i < BOARD_ROWS; i++)
    {
        int colCount = 0;
        for (int j = 0; j < BOARD_COLS; j++)
        {
            if(state.board[i][j] == 1)
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

void collapseRow(int index)
{
    for (int i = index; i > 0; i--)
    {
        for (int j = 0; j < BOARD_COLS; j++)
        {
            state.board[i][j] = state.board[i - 1][j];
        }
    }
}

int getNextRotationIndex()
{
    if (state.shape.rotationIndex != state.shape.rotationCount - 1)
    {
        return state.shape.rotationIndex + 1;
    }

    return 0;
}

void rotate()
{
    // state.shape.rotationIndex = getNextRotationIndex();

    transpose(state.shape.rotation[0]);
    reverseRows(state.shape.rotation[0]);
}

bool checkCollision(int dx, int dy, int rotationIndex)
{
    for (int row = 0; row < 4; row++)
    {
        for (int col = 0; col < 4; col++)
        {
            if (state.shape.rotation[rotationIndex][row][col] == 1)
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

    return false;
}

bool checkGroundedCollision(int dx, int dy)
{
    for (int row = 0; row < 4; row++)
    {
        for (int col = 0; col < 4; col++)
        {
            if (state.shape.rotation[state.shape.rotationIndex][row][col] == 1)
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
    state.shape.x = 0;
    state.shape.y = 0;
    state.shape.rotationIndex = 0;
}

void applyShapeToBoard()
{
    for (int row = 0; row < 4; row++)
    {
        for (int col = 0; col < 4; col++)
        {
            if (state.shape.rotation[state.shape.rotationIndex][row][col] == 1)
            {
                int x = col + state.shape.x;
                int y = row + state.shape.y;

                state.board[y][x] = 1;
            }
        }
    }
}


void updateGame()
{
    if (!state.started)
    {
        return;
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

    if(row != 0)
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
            if (state.shape.rotation[state.shape.rotationIndex][row][col] == 1)
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

    state.board[19][0] = 1;
    state.board[19][1] = 1;
    state.board[19][2] = 1;
    state.board[19][3] = 1;
    state.board[19][4] = 1;
    state.board[19][5] = 1;
    state.board[19][6] = 1;
    state.board[19][7] = 1;
    state.board[19][8] = 1;
    state.board[19][9] = 1;
    state.board[18][1] = 1;

    state.board[18][0] = 1;
    state.board[18][1] = 1;
    state.board[18][2] = 1;
    state.board[18][3] = 1;
    state.board[18][4] = 1;
    state.board[18][5] = 1;
    state.board[18][6] = 1;
    state.board[18][7] = 1;
    state.board[18][8] = 1;
    state.board[18][9] = 1;
    state.board[18][1] = 1;

    state.board[18][5] = 1;
    state.board[17][0] = 1;

    state.tick_rate = 1000 / TICK_RATE;

    // int iRotations[2][4][4] = {
    //     {
    //         {0, 0, 0, 0,},
    //         {0, 0, 0, 0,},
    //         {1, 1, 1, 1,},
    //         {0, 0, 0, 0,}
    //     },
    //     {
    //         {0, 0, 1, 0,},
    //         {0, 0, 1, 0,},
    //         {0, 0, 1, 0,},
    //         {0, 0, 1, 0,}
    //     },
    // };

    int iRotations[4][4][4] = {
        {
            {0, 0, 0, 0,},
            {1, 1, 1, 0,},
            {0, 0, 1, 0,},
            {0, 0, 0, 0,}
        },
        {
            {0, 1, 0, 0,},
            {0, 1, 0, 0,},
            {1, 1, 0, 0,},
            {0, 0, 0, 0,}
        },

        {
            {1, 0, 0, 0,},
            {1, 1, 1, 0,},
            {0, 0, 0, 0,},
            {0, 0, 0, 0,}
        },
        {
            {0, 1, 1, 0,},
            {0, 1, 0, 0,},
            {0, 1, 0, 0,},
            {0, 0, 0, 0,}
        }


    };

    state.shape.rotationCount = 4;
    state.shape.rotationIndex = 0;
    state.shape.x = 0;
    state.shape.y = 0;
    state.shape.rotation = (int (*)[4][4])malloc(sizeof(int) * 4 * 4 * 4);

    for (int r = 0; r < 4; r++)
    {
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                state.shape.rotation[r][i][j] = iRotations[r][i][j];
            }
        }
    }
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
        if (checkCollision(0, 0, getNextRotationIndex()))
        {
            return;
        }
        rotate();
        break;
    case 's': // Move down
        if (checkCollision(0, 1, state.shape.rotationIndex))
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
        if (checkCollision(-1, 0, state.shape.rotationIndex))
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
        if (checkCollision(1, 0, state.shape.rotationIndex))
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
