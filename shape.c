#include <stdlib.h>

typedef struct
{
    int x, y;
    int (*rotation)[4][4];
} Shape;



Shape* initShapes()
{
    int iRotations[2][4][4] = {
        {
            {0, 0, 0, 0,},
            {0, 0, 0, 0,},
            {1, 1, 1, 1,},
            {0, 0, 0, 0,}
        },
        {
                {0, 0, 1, 0,},
                {0, 0, 1, 0,},
                {0, 0, 1, 0,},
                {0, 0, 1, 0,}
        },
    };


    Shape* shapes = (Shape*)malloc(sizeof(Shape) * 1);

    shapes[0].rotation = (int (*)[4][4])malloc(sizeof(int) * 2 * 4 * 4);

    for (int r = 0; r < 2; r++) {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                shapes[0].rotation[r][i][j] = iRotations[r][i][j];
            }
        }
    }

    return shapes;
}
