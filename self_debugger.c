#include "self_debugger.h"

void feedback(char *msg)
{
    pthread_mutex_lock(&err_mamutex);
    if (ERRSTR.errsize + strlen(msg) + strlen("ERROR FREE MEMORY OUT") >= ERRSIZE)
    {
        if (ERRSTR.errsize != ERRSIZE)
        {
            sprintf(ERRSTR.ERRTAB + ERRSTR.errsize, "ERROR FREE MEMORY OUT");
            ERRSTR.errsize = ERRSIZE;
        }
        return (void)pthread_mutex_unlock(&err_mamutex);
    }
    sprintf(ERRSTR.ERRTAB + ERRSTR.errsize, "%s\n", msg);
    ERRSTR.errsize += strlen(msg) + 1;
    pthread_mutex_unlock(&err_mamutex);
}
void display_errs()
{
    if (!ERRSTR.errsize)
        printf("No errors. Great!");
    else
        printf("%s", ERRSTR.ERRTAB);
}
