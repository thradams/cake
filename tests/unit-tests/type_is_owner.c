#pragma safety enable


struct Y { 
    char  * _Owner owner_variable;   
    char  * non_owner_variable;   
};

struct X { 
    char  * _Owner owner_variable;   
    char  * non_owner_variable;   
    struct Y y1;
    _View struct Y y2;
};

void f()
{
    struct X x;
    
    static_assert(_is_owner(typeof(x)));
    static_assert(_is_owner(typeof(x.owner_variable)));
    static_assert(!_is_owner(typeof(x.non_owner_variable)));
    static_assert(_is_owner(struct X));
    static_assert(_is_owner(typeof(x.y1)));
    static_assert(!_is_owner(typeof(x.y2)));
    
    static_assert(_is_owner(typeof(x.y1.owner_variable)));
    static_assert(!_is_owner(typeof(x.y1.non_owner_variable)));

    static_assert(!_is_owner(typeof(x.y2.owner_variable)));
    static_assert(!_is_owner(typeof(x.y2.non_owner_variable)));

    _View struct X x2;
    static_assert(!_is_owner(typeof(x2)));
    static_assert(!_is_owner(typeof(x2.owner_variable)));
    static_assert(!_is_owner(typeof(x2.non_owner_variable)));

    _Owner char * p;
    static_assert(!_is_owner(typeof(p)));
    static_assert(_is_owner(typeof(*p)));    
}
