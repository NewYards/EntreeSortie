#include "stdes.h"

#define BUFFER_SIZE 10000000
#include <stdio.h>

// Fonction pour tester les fonctions
int main(int argc, char *argv[])
{
	FICHIER *f1;
	FICHIER *f2;
    FICHIER *f3;
	char c;

    // Verification du nombre d'entrée fournis
	if (argc != 4) exit(-1);

    // Test fonction ouvrir
	f1 = ouvrir (argv[1], 'L');
	if (f1 == NULL) exit (-1);
    else printf("Ouverture d'un fichier en lecture réussi.\n");
    f2 = ouvrir (argv[2], 'E');
	if (f2 == NULL) exit (-1);
    else printf("Ouverture d'un fichier existant en écriture réussi.\n");
    f3 = ouvrir (argv[3], 'E');
	if (f3 == NULL) exit (-1);
    else printf("Ouverture d'un fichier créer en écriture réussi.\n");

    // Test fonction vider
    int fd;
    // Test cas ecriture avec load_data == cursor
    fd = f2->fd;
    f2->fd = 1;
    vider(f2);
    f2->fd = fd;

    // Test cas ecriture avec load_data > cursor 
    f2->buffer[0] = 'O';
    f2->buffer[1] = 'k';
    f2->buffer[2] = '\n';
    f2->load_data = f2->load_data + 3;
    fd = f2->fd;
    f2->fd = 1;
    vider(f2);
    f2->fd = fd;

    // Test cas lecture avec load_data == cursor
    fd = f1->fd;
    f1->fd = 1;
    vider(f1);
    f1->fd = fd;

    // Test cas lecture avec load_data > cursor
    f1->buffer[0] = 'N';
    f1->buffer[1] = 'o';
    f1->buffer[2] = '\n';
    f1->buffer[3] = 'O';
    f1->buffer[4] = 'k';
    f1->buffer[5] = '\n';
    f1->load_data = f1->load_data + 6;
    f1->cursor = f1->cursor + 3;
    fd = f1->fd;
    f1->fd = 1;
    vider(f1);
    f1->fd = fd;
    printf("%c", f1->cursor[0]);
    printf("%c", f1->cursor[1]);
    printf("%c", f1->cursor[2]);

    // Test fonction fermer
    if(fermer (f1)==-1)exit(-1);
    else printf("Fermeture d'un fichier en lecture réussi.\n");
    if(fermer (f2)==-1)exit(-1);
    else printf("Fermeture d'un fichier existant en écriture réussi.\n");
    if(fermer (f3)==-1)exit(-1);
    else printf("Fermeture d'un fichier créer en écriture réussi.\n");
	return 0;
}

FICHIER *ouvrir(const char *nom, char mode)
{
    // On met fd a -1 pour le cas ou mode est différent de E et L
    int fd = -1;
    // On ouvre le fichier selon le mode passer en paramètre
    if(mode == 'E') fd = open(nom, O_WRONLY | O_CREAT);
    if(mode == 'L') fd = open(nom, O_RDONLY);
    // Si le mode est différent de E ou L ou que l'ouverture a echoué on renvoit -1
    if(fd < 0) return NULL;
    // On alloue et on intialise les variable dans un nouveau FICHIER
    FICHIER * file = malloc(sizeof(FICHIER));
    file->size = BUFFER_SIZE;
    file->buffer = malloc(file->size);
    file->cursor = file->buffer;
    file->mode = mode;
    file->load_data = file->buffer;
    file->fd = fd;
    // Enfin on renvoit le FICHIER créer
    return file;
}

int vider(FICHIER *f)
{
    int size_remaining = f->load_data - f->cursor;
    // Si le fichier est ouvert en ecriture on vide le buffer dans le fichier
    if(f->mode == 'E' && size_remaining > 0) write(f->fd, f->cursor, size_remaining);

    // Si le fichier est ouvert en lecture on vide le buffer des 
    if(f->mode == 'L' && size_remaining > 0) memcpy(f->buffer, f->cursor, size_remaining);
        
    f->cursor = f->buffer;
    f->load_data = f->buffer;
    return 0;
}

int fermer(FICHIER*f)
{
    if(f->mode == 'E' && f->load_data != f->buffer) vider(f);

    if(close(f->fd)) return -1;

    free(f->buffer);
    free(f);  

    return EXIT_SUCCESS; 
}
