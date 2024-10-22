#include <stdbool.h>
#include <stdlib.h>
#include <GL/freeglut_std.h>

bool check_overlap(double x1, double y1, double s1, double x2, double y2, double s2) {
    double right1 = x1 + s1;  // Right edge of square 1
    double top1 = y1 + s1;    // Top edge of square 1
    double right2 = x2 + s2;  // Right edge of square 2
    double top2 = y2 + s2;    // Top edge of square 2

    if (x1 >= right2 || x2 >= right1 || y1 >= top2 || y2 >= top1) {
        return false;
    }

    return true;
}

void renderBitmapString(float x, float y, void *font, const char *string) {
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(x, y);  // Set the position for the text

    while (*string) {
        glutBitmapCharacter(font, *string);  // Render each character
        string++;
    }
}

int random(int min, int max)
{
    return rand() % (max + 1 - min) + min;
}