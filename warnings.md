## Warnings

Warnings can be controlled with "-Wname" and "-Wno-name" and "-Wall".
When the same warning exists in gcc cakes uses the same name if possible.

Inside source code you can control warning like this

```c

enum E1 { A };
enum E2 { B };

int main() {

#pragma CAKE diagnostic push
#pragma CAKE diagnostic ignore "-Wenum-compare"
    if (A == B){}
#pragma CAKE diagnostic pop

    if (A == B) {}

}

```

### -Wunused-variable
### -Wdeprecated
### -Wenum-compare
### -Wnonnull
### -Waddress
### -Wunused-parameter
### -Whide-declarator
### -Wtypeof-parameter
### -Wattributes
### -Wunused-value

### -Wstyle


|C23 feature                    |Paper(s) |Cake |
|-------------------------------|---------|-----|
|_Static_assert with no message | [N2265](https://open-std.org/JTC1/SC22/WG14/www/docs/n2265.pdf)  |     |   
|                               |         |     |   
|                               |         |     |   




