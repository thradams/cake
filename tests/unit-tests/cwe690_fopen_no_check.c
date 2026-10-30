/* CWE690 Juliet CWE690_NULL_Deref_From_Return__fopen_01 */

#pragma safety enable

struct FILE;
struct FILE* _Owner fopen(const char* name, const char* mode);
int fclose(struct FILE* _Owner p);

void bad()
{
    struct FILE* _Owner f = fopen("file.txt", "w+");
    fclose(f); //lint 35
}
