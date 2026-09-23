#include <stdlib.h>
#include <stdio.h>

void async(void * capture, void callback(void* capture, int result))
{
   callback(capture, 1);
}

int main()
{
	struct capture {int value; }* capture = calloc(1, sizeof * capture);
    if (capture == 0) return 1;

    capture->value = 123;
    async(capture, (static void (void * capture, int result))
    {
		struct capture* p = capture;
        printf("result=%d, value=%d\n", result, p->value);
        free(p);
    });
}
