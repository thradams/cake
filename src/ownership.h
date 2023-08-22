
#pragma once



#ifdef __CAKE__
void* owner calloc(int nmemb, int size);
void free(void* owner ptr);
void* owner malloc(int size);
void* owner realloc(void* owner ptr, int size);
 char * owner strdup( const char *src );
int fclose(FILE* owner _Stream);

char * owner strdup( const char *str1 );
#define unchecked "unchecked"
#else
#define owner
#define obj_owner
#define view
#define static_debug(x)
#define unchecked
#endif

