#include "self_debugger.h"

void feedback(char *msg)
{
    pthread_mutex_lock(&err_mamutex);
    if (ERRSTR.errsize + strlen(msg) >= ERRSIZE)
        return (void)pthread_mutex_unlock(&err_mamutex);
    sprintf(ERRSTR.ERRTAB + ERRSTR.errsize, "%s\n", msg);
    pthread_mutex_unlock(&err_mamutex);
}
void display_errs() 
{
    if (!ERRSTR.errsize)
        printf("No errors. Great!");
    else
        printf(ERRSTR.ERRTAB);
}
