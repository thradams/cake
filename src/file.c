#pragma once

#ifdef __CAKE__
#pragma CAKE diagnostic push
#pragma CAKE diagnostic ignore "-Wstyle"
#endif

#define try  
#define catch if (0) catch_label:
#define throw goto catch_label

#ifdef __CAKE__
#pragma CAKE diagnostic pop
#endif

int main()
{
}
