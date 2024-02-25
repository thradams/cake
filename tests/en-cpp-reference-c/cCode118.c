//en.cppreference.com/w/c/language/operator_alternative.html
%:include <stdio.h>
%:include <stdlib.h>
??=include <iso646.h>
 
int main(int argc, char** argv)
??<
    if (argc > 1 and argv<:1:> not_eq NULL)
    <%
       printf("Hello %s??/n", argv<:1:>);
    %>
    else
    <%
       printf("Hello %s??/n", argc? argv??(42??'42??) : __FILE__);
    %>
 
    return EXIT_SUCCESS;
??>