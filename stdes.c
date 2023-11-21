#include "stdes.h"

#define BUFFER_SIZE 10000000
#include <stdio.h>

// Fonction pour tester les fonctions
int main(int argc, char *argv[])
{
	FICHIER *f1;
	FICHIER *f2;
	char *c1 = malloc(sizeof(char) * 7);
    char *c2 = malloc(sizeof(char) * 7);
    char *c3 = malloc(sizeof(char) * 7);
    char *c4 = malloc(sizeof(char) * 7);
    char *c5 = malloc(sizeof(char) * 7);
    char *c6 = malloc(sizeof(char) * 7);

    // Verification du nombre d'entrée fournis
	if (argc != 3) exit(-1);

    // Test lire un fichier en ecriture
    if ((f2 = ouvrir (argv[2], 'E')) == NULL) exit (-1);
    if(lire(&c5, 1, 1, f2) == -1) printf("Lecture d'un fichier ouvert en ecriture refusé.\n");
    else exit(-1);
    if(fermer (f2)==-1)exit(-1);


    ssize_t size_read;
    // Test de la fonction de lecture
    // Cas buffer vide et size_wanted < f->size
    printf("Cas buffer vide et size_wanted < f->size\n");
    if ((f1 = ouvrir (argv[1], 'L')) == NULL) exit (-1);
    size_read = lire(c1, 1, 3, f1);
    printf("    Taille lus : %d\n", size_read);
    printf("    Octets lus : %s\n", c1);
    if(fermer (f1)==-1)exit(-1);

    // Cas buffer vide et size_wanted > f->size (On dit f->size car dans notre implementations c'est egale a size_remainig qui ici est egale a ce qu'on a lus, amélioration possible)
    printf("Cas buffer vide et size_wanted > f->size\n");
    if ((f1 = ouvrir (argv[1], 'L')) == NULL) exit (-1);
    size_read = lire(c2, 1, 10, f1);
    printf("    Taille lus : %d\n", size_read);
    printf("    Octets lus : %s\n", c2);
    if(fermer (f1)==-1)exit(-1);

    // Cas buffer non vide et size_wanted < f->size
    printf("Cas buffer non vide et size_wanted < f->size\n");
    if ((f1 = ouvrir (argv[1], 'L')) == NULL) exit (-1);
    size_read = lire(c3, 1, 1, f1);
    printf("    Taille lus : %d\n", size_read);
    printf("    Octets lus : %s\n", c3);
    size_read = lire(c3, 1, 1, f1);
    printf("    Taille lus : %d\n", size_read);
    printf("    Octets lus : %s\n", c3);
    if(fermer (f1)==-1)exit(-1);

    // Cas buffer non vide et size_wanted > f->size
    printf("Cas buffer non vide et size_wanted > f->size\n");
    if ((f1 = ouvrir (argv[1], 'L')) == NULL) exit (-1);
    size_read = lire(c4, 1, 1, f1);
    printf("    Taille lus : %d\n", size_read);
    printf("    Octets lus : %s\n", c4);
    size_read = lire(c4, 1, 10, f1);
    printf("    Taille lus : %d\n", size_read);
    printf("    Octets lus : %s\n", c4);
    if(fermer (f1)==-1)exit(-1);

    // Cas lire un element superieur a la taille du fichier
    printf("Cas buffer non vide et size_wanted > f->size\n");
    if ((f1 = ouvrir (argv[1], 'L')) == NULL) exit (-1);
    size_read = lire(c6, 10, 1, f1);
    printf("    Taille lus : %d\n", size_read);
    printf("    Octets lus : %s\n", c6);
    if(fermer (f1)==-1)exit(-1);

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
        
    // On reset la position du curseur et des données lus
    f->cursor = f->buffer;
    f->load_data = f->buffer;
    return 0;
}

int fermer(FICHIER*f)
{
    // Si le fichier est ouvert en ecriture et que il reste des données dans le buffer on vide le buffer
    if(f->mode == 'E' && f->load_data != f->buffer) vider(f);

    // On ferme le descripteur de fichier, si il y a une erreur on retourn -1
    if(close(f->fd)) return -1;

    // On libere la memoire alloué dans notre stucture
    free(f->buffer);
    free(f);  

    return EXIT_SUCCESS; 
}

ssize_t lire(void *p, unsigned int taille, unsigned int nbelem, FICHIER *f)
{
    // Si le fichier est ouvert en ecriture ou que la destionation est NULL on retourne -1
    if(f->mode != 'L' && p != NULL) return -1;

    // Initialisation
    // size wanted est la taille maximum que l'on va lire
    int size_wanted = (taille*nbelem);
    // elem_read est le compteur d'element que l'on a lus
    ssize_t elem_read = 0;
    // sier remaining est la taille des données dans le buffer qui n'ont pas encore etais lus
    int size_remaining = f->load_data - f->cursor;

    // Si le buffer est vide et que ce que l'on veut lire est inferieur a la taille du buffer on remplit le buffer au maximum
    if(size_remaining <= 0 && size_wanted <= f->size)
    {
        // size loaded est le nombre d'octets que read a lu
        int size_loaded = read(f->fd, f->buffer, f->size);
        // Si on n'a pas lu d'octet on renvoit 0 car notre buffer est vide et nous somme a la fin du fichier
        if(size_loaded == 0) return 0;
        else if (size_loaded < 0) return -1;
        // On augmente load_data de size_loaded octets
        f->load_data = f->load_data + size_loaded;
        // On reset le curseur au debut du buffer
        f->cursor = f->buffer;
        // On recalcul size_remaining, sachant qu'il ne peut plus etre egale a 0.
        size_remaining = f->load_data - f->cursor;
    }

    if(size_wanted <= size_remaining)
    {
        // On copie les elements du buffer dans p
        for(int i = 0; i < nbelem; i++) 
        { 
            memcpy(p+(i*taille), f->cursor, taille);
            elem_read = elem_read + 1;
            f->cursor = f->cursor + taille;
        }
        // Cas ou on a lu tous les donnée dans le buffer, on reset le curseur et les donnée charger.
        if((elem_read*taille) == size_remaining && f->cursor == f->load_data)
        {
            f->cursor = f->buffer;
            f->load_data = f->buffer;
        }
    }
    else if(size_wanted > size_remaining)
    {
        // Dans un premier temps on vide le buffer d'un maximum d'element
        while(size_remaining >= taille)
        {
            memcpy(p+(elem_read*taille), f->cursor, taille);
            elem_read = elem_read + 1;
            f->cursor = f->cursor + taille;
            size_remaining = size_remaining - taille;
        }

        // Dans un second temps on creer un buffer qui va contenir le reste des elements et de quoi re remplir le buffer
        void * tmp_buffer = malloc(taille*(nbelem-elem_read) + f->size);
        int size_loaded = read(f->fd, tmp_buffer+size_remaining, ((taille*(nbelem-elem_read)) - size_remaining) + f->size);
        if (size_loaded < 0) return -1;
        if(tmp_buffer == NULL) return -1;

        // Cependant il est possible qu'un element soit couper en 2 entre le buffer et le fichier donc on stock au debut de tmp buffer la premiere partie
        if(size_remaining != 0 && size_remaining + size_loaded >= taille) memcpy(tmp_buffer, f->cursor, size_remaining);
        // Si il ne reste pas assez d'octets dans le fichier pour faire un element avec le reste du buffer
        else if (size_remaining != 0 && size_remaining + size_loaded < taille) return elem_read;
        // Si il ne reste pas assez d'octets dans le fichier pour faire un element
        else if (size_remaining + size_loaded < taille) return elem_read;
        
        // Le buffer etant vide on reset curseur et donnée charger
        f->cursor = f->buffer;
        f->load_data = f->buffer;

        // On creer un nouveau compteur pour les elements lu dans le nouveau buffer
        ssize_t elem_read_in_file = 0;
        // On lis les elements restant
        while(elem_read + elem_read_in_file < nbelem && size_loaded >= taille) // = ou pas ?
        {
            memcpy(p+((elem_read_in_file+elem_read)*taille), tmp_buffer+(elem_read_in_file*taille), taille);
            elem_read_in_file = elem_read_in_file + 1;
            size_loaded = size_loaded - taille;
        }
        // On ajoute les 2 compteur
        elem_read = elem_read + elem_read_in_file;

        // Si apres avoir lu tous les elements il reste des donnée on les place dans le buffer
        if(size_loaded > 0) memcpy(f->buffer, tmp_buffer+(elem_read_in_file*taille), size_loaded);
        // Enfin on libre le buffer temporaire
        free(tmp_buffer);
    }
    return elem_read;
}
