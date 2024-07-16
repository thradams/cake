#pragma safety enable


struct Y { 
    char  * /*_Owner*/ owner_variable;   
    char  * non_owner_variable;   
};

struct X { 
    char  * /*_Owner*/ owner_variable;   
    char  * non_owner_variable;   
    struct Y y1;
    _View struct Y y2;
};

void f()
{
    struct X x;
    
    
    
    
    
    
    
    
    
    

    
    

    /*_View*/ struct X x2;
    
    
    

    /*_Owner*/ char * p;
    
        
}
