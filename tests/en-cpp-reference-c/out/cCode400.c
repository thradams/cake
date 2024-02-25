//en.cppreference.com/w/c/string/wide/wcsrchr.html
#include <locale.h>
#include <stdio.h>
#include <wchar.h>
 
int main(void)
{
    wchar_t arr[] = L"白猫 黒猫 кошки";
    wchar_t *cat = wcsrchr(arr, L'猫');
    wchar_t *dog = wcsrchr(arr, L'犬');
 
    setlocale(LC_ALL, "en_US.utf8");
 
    cat ? printf("The character 猫 found at position %td\n", cat - arr)
        : puts("The character 猫 not found");
 
    dog ? printf("The character 犬 found at position %td\n", dog - arr)
        : puts("The character 犬 not found");
}