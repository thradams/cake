
#pragma once



#ifdef __CAKE__

#define _destroy [[destroy]]
#define _delete [[destroy,free]]
#define _free [[free]]

#else

#define _destroy 
#define _delete
#define _free

#define _del_attr(a, b)
#define _add_attr(a, b)

#endif
