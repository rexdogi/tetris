#ifndef TIME_H
#define TIME_H
int clock_gettime(int clock_id, struct timespec *spec);
double getCurrentTime();
#endif // TIME_H
