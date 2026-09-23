/*
   Pattern:
   do this -> then that -> then that ....
*/

#include <stdlib.h>
#include <stdio.h>

void login_async(void * data, void callback(void* data, int id))
{
   callback(data, 1);
}

void get_data_async(void * data, void callback(const char* email, void * data))
{
  callback(data, "your data...");
}

int main()
{
	struct capture { int id; }* capture = calloc(1, sizeof * capture);
    login_async(capture, (static void (int id, void * capture))
    {
        struct capture * cap1 = capture;

        printf("login completed. id=%d\n", id);
        cap1->id = id;
        get_data_async(cap1, (static void (const char* email, void * data))
        {
		    struct capture * cap2 = data;
            printf("your data='%s'  from id=%d\n", email, cap2->id);
            free(cap2);
        });
    });
}
