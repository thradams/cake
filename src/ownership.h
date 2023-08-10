
#pragma once



#ifdef __CAKE__
#define implicit _Implicit
#define owner _Owner
#define obj_owner _Obj_owner
#define move _Move
#define view _View


[[nodiscard]] void* _Owner calloc(int nmemb, int size);
void free(_Implicit void* _Owner ptr);
[[nodiscard]] void* _Owner malloc(int size);
[[nodiscard]] void* _Owner realloc(void* _Owner ptr, int size);
[[nodiscard]] char * _Owner strdup( const char *src );
char * _Owner strdup( const char *str1 );

#else
#define implicit
#define owner
#define obj_owner
#define move
#define view
#define static_debug(x)
#endif
