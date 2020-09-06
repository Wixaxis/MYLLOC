#ifndef IM_BUGGED
#define IM_BUGGED

#include <pthread.h>
#include <stdio.h>
#include <string.h>

#define ERRSIZE 10000

struct err_t
{
    char ERRTAB[ERRSIZE];
    int errsize;
} ERRSTR;

pthread_mutex_t err_mamutex;

void feedback(char *msg);
void display_errs();

#endif