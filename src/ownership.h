
#pragma once



#ifdef __CAKE__
#define implicit _Implicit
#define owner _Owner
#define obj_owner _Obj_owner
#define move _Move


[[nodiscard]] void* owner calloc(int nmemb, int size);
void free(implicit void* owner ptr);
[[nodiscard]] void* owner malloc(int size);
[[nodiscard]] void* owner realloc(void* owner ptr, int size);



#else
#define implicit
#define owner
#define obj_owner
#define move

#endif
