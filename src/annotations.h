
#pragma once



#ifdef __CAKE__

#define _destroy [[cake::destroy]]
#define _delete [[cake::destroy,cake::free]]
#define _free [[cake::free]]
#define MOVE [[cake::move]]
#else

#define _destroy 
#define _delete
#define _free

#define _del_attr(a, b)
#define _add_attr(a, b)
#define MOVE

#endif
