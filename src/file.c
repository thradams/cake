
/*
  This code is from 
  The C Programming Language 2 edition, page 145

  There are two bugs int this original sample
  - one memory leak
  - one invalid state
*/

#pragma safety enable

#include <stdlib.h>
#include <string.h>

struct nlist {          /* table entry: */
    struct nlist *next; /* next entry in chain */
    char *name;  /* defined name */
    char *defn;  /* replacement text */
};

struct nlist *lookup(char *s);

/* hash:  form hash value for string s */
unsigned hash(char *s);


#define HASHSIZE 101

static struct nlist *hashtab[HASHSIZE]; /* pointer table */

/*1* lookup: look for s in hashtab */
struct nlist *lookup(char *);


/* install:  put (name, defn) in hashtab */
struct nlist *install(char *name, char *defn)
{
    struct nlist *np;
    unsigned hashval;

    if ((np = lookup(name)) == NULL) {  /* not found */
        np = (struct nlist *) malloc(sizeof(*np));
        if (np == NULL || (np->name = strdup(name)) == NULL)
            return NULL;
        hashval = hash(name);
        np->next = hashtab[hashval];
        hashtab[hashval] = np;
    } else      /* already there */
        free((void *) np->defn);  /* free previous defn */

    if ((np->defn = strdup(defn)) == NULL)
        return NULL;
    return np;
}
