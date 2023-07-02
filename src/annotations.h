
#pragma once



#ifdef __CAKE__
#define implicit [[cake::implicit]]
#else
#define implicit
#define _owner
#define _view
#define _obj_owner
#define _move

#endif
