#include <stdlib.h>

void async(void callback(int result, void * data), void * data);

int main()
{
	struct {int value; }* capture = calloc(1, sizeof * capture);

    static void callback(int result, void * data)
    {
		typeof(capture) p = data;
        free(p);
    }

    async(callback, capture);
}
