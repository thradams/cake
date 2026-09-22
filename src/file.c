#include <threads.h> 
 
static mtx_t mtx; 
static cnd_t cnd; 
static int condition; 
 
int main(void) { 
    if(mtx_lock(&mtx) != thrd_success) { 
        return 1; 
    } 
    while(condition == 0) { 
        if(cnd_wait(&cnd, &mtx) != thrd_success) { 
            return 1; 
        } 
    } 
    mtx_unlock(&mtx); 
    return 0; 
} 
