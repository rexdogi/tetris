#include <time.h>
#include <windef.h>
#include <GL/freeglut_std.h>
#define CLOCK_MONOTONIC 0

int clock_gettime(int clock_id, struct timespec *spec) {
    static LARGE_INTEGER frequency;
    static BOOL initialized = FALSE;
    LARGE_INTEGER count;

    if (!initialized) {
        QueryPerformanceFrequency(&frequency);  // Get ticks per second
        initialized = TRUE;
    }

    QueryPerformanceCounter(&count);  // Get current tick count

    // Convert ticks to seconds and nanoseconds
    spec->tv_sec = count.QuadPart / frequency.QuadPart;
    spec->tv_nsec = (long)((count.QuadPart % frequency.QuadPart) * 1e9 / frequency.QuadPart);

    return 0;
}

double getCurrentTime() {
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return now.tv_sec * 1000.0 + now.tv_nsec / 1000000.0;
}