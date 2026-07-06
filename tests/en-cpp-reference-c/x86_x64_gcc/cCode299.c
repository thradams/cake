/* Cake x86_x64_gcc */

extern char *getenv(char * __name);
extern int printf(char * __format, ...);

int main(void)
{
    char * name;
    char * env_p;

    name = "PATH";
    env_p = getenv(name);
    if (env_p)
    {
        printf("Your %s is %s\n", name, env_p);
    }
}


