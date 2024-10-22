#pragma once
typedef struct
{
    int x, y;
    int (*rotation)[4][4];
    int rotationIndex;
    int rotationCount;
    int body[4][4];

} Shape;

Shape* initShapes();