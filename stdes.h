#ifndef _STDES_H
#define _STDES_H

#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h> 
#include <stdarg.h>

#include <limits.h>

typedef struct _ES_FICHIER
{
    int fd;
    char mode;
    char * buffer;
    char * cursor;
    size_t size;
    char * load_data;
} FICHIER;

extern FICHIER *stdout_io;
extern FICHIER *stderr_io;

/* mode: 'L' = lecture, 'E' = écriture */
FICHIER *ouvrir(const char *nom, char mode);
int fermer(FICHIER*f);
ssize_t lire(void *p, unsigned int taille, unsigned int nbelem, FICHIER *f);
ssize_t ecrire(const void *p, unsigned int taille, unsigned int nbelem, FICHIER *f);
int vider(FICHIER *f);

int fecriref (FICHIER *f, const char *format, ...);
/* directly in stdout */
int ecriref (const char *format, ...);
int fliref (FICHIER *f, const char *format, ...);


#endif
