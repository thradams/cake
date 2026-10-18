/* CWE775 Juliet CWE775_Missing_Release_of_File_Descriptor_or_Handle__fopen_no_close_01 */

#pragma safety enable

struct FILE;
struct FILE* _Owner _Opt fopen(const char* name, const char* mode);

void bad()
{
    struct FILE* _Owner _Opt f = fopen("file.txt", "w+");
} //lint 29
