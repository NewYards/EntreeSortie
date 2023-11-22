#include "stdes.h"

#define BUFFER_SIZE 10
#include <stdio.h>

FICHIER *stdout_io = NULL;
FICHIER *stderr_io = NULL;

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

    // Si le fichier est ouvert en lecture on vide le buffer des données deja lus
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
        while(elem_read + elem_read_in_file < nbelem && size_loaded >= taille) 
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

ssize_t ecrire(const void *p, unsigned int taille, unsigned int nbelem, FICHIER *f)
{
    // Si le fichier est ouvert en lecture ou que la source est NULL on retourne -1
    if(f->mode != 'E' && p != NULL) return -1;

    // Initialisation
    // size wanted est la taille maximum que l'on va ecrire
    int size_wanted = (taille*nbelem);
    // elem_write est le compteur d'element que l'on a ecrit
    ssize_t elem_write = 0;
    // size remaining est la taille vide du buffer
    int size_remaining = (f->buffer + f->size) - f->load_data;
    // size_of_p est la taille du tableau p
    int size_of_p = 0;
    // tmp_ptr est un pointeur temporaire pour trouver la taille de p
    char * tmp_ptr = (char *)p;
    while(*tmp_ptr != '\0') 
    {
        size_of_p = size_of_p + 1;
        tmp_ptr = (tmp_ptr + 1);
    }
    // Si p est plus petit que la taille demandé on calcul le nombre maximum d'element dans p
    while(size_of_p < size_wanted) 
    {
        nbelem = nbelem - 1;
        size_wanted = (taille*nbelem);
    }

    // Si le buffer est remplit et que ce que l'on veut ecrire est inferieur a la taille du buffer on vide le buffer
    if(size_remaining <= size_wanted && size_wanted <= f->size)
    {
        // size loaded est le nombre d'octets que write a ecrit
        int size_loaded = write(f->fd, f->cursor, f->size - size_remaining);
        if (size_loaded < 0) return -1;
        // On reset load_data
        f->load_data = f->buffer;
        // On reset le curseur au debut du buffer
        f->cursor = f->buffer;
        // On recalcul size_remaining, sachant qu'il ne peut plus etre egale a 0.
        size_remaining = (f->buffer + f->size) - f->load_data;
    }
    if(size_wanted < f->size) 
    {
        // On copie les elements de p dans le buffer
        for(int i = 0; i < nbelem; i++) 
        { 
            memcpy(f->load_data, p+(i*taille), taille);
            elem_write = elem_write + 1;
            f->load_data = f->load_data + taille;
        }
    }
    else if(size_wanted >= f->size) 
    {
        // size loaded est la taille qui n'a pas encore etais ecrite dans le fichier
        int size_loaded = f->load_data - f->cursor;
        // tmp_buffer est un buffer qui va contenir les element du buffer du fichier suivit des elements nouveaux demandée a ecrire sur le fichier
        void * tmp_buffer = malloc(size_wanted + size_loaded);
        // On ecrit au debut de tmp_buffer f->cursor jusqua f->load_data
        memcpy(tmp_buffer, f->cursor, size_loaded);
        // On reset le curseur et load_data
        f->cursor = f->buffer;
        f->load_data = f->buffer;
        // On ecrit les nouveaux element dans tmp_buffer 
        while(elem_write < nbelem) 
        {
            memcpy(tmp_buffer + size_loaded + (elem_write*taille), p+(elem_write*taille), taille);
            elem_write = elem_write + 1;
        }
        // Enfin on ecrit tmp_buffer dans le fichier
        write(f->fd, tmp_buffer, size_loaded + size_wanted);
        free(tmp_buffer);
    }
    return elem_write;
}

int fecriref (FICHIER *f, const char *format, ...)
{
    if(f->mode != 'E') return -1;

    // On commence par calculer la taille en caratere du plus grand int
    int int_max = INT_MAX;
    int max_size_int = 0;
    while(int_max >= 10)
    {
        int_max = int_max/10;
        max_size_int ++;
    }
    // Pour le signe - dans le cas ou le nombre est negatif
    max_size_int++;
    // On initialise un va_list
    va_list args;
    va_start(args, format);
    // size_res est la taille en octets (char) de notre chaine resultante du format
    int size_res = 0;
    // On recupere la taille de format
    int size_format = strlen(format);

    // args_int va contenir tous les argument de type int du format dans l'ordre
    int* args_int = malloc(sizeof(int) * (size_format/2));
    // args_int_compteur permet de se souvenir ou on se trouve dans args_int
    int args_int_compteur = 0;
    // args_char va contenir tous les argument de type char du format dans l'ordre
    char* args_char = malloc(sizeof(char) * (size_format/2));
    // args_char_compteur permet de se souvenir ou on se trouve dans args_char
    int args_char_compteur = 0;
    // args_char_ptr va contenir tous les argument de type char* du format dans l'ordre
    char** args_char_ptr = malloc(sizeof(char*) * (size_format/2));
    // args_char_ptr_compteur permet de se souvenir ou on se trouve dans args_char_ptr
    int args_char_ptr_compteur = 0;

    // On recupere ttous les arguments et on calcul la somme de la taille de tous les argument plus les caractere fixe de format
    for(int i = 0; i < size_format; i++)
    {
        if(format[i] == '%')
        {
            if(format[i+1] == 'd')
            {
                args_int[args_int_compteur] = va_arg(args, int);
                int tmp = args_int[args_int_compteur];
                int size_int = 0;
                //On calcul la taille en caractere de l'argument de type int
                if(tmp<0)
                { 
                    size_int ++; 
                    tmp = tmp*-1;
                }
                while(tmp >= 10)
                {
                    tmp = tmp/10;
                    size_int ++;
                }
                size_res = size_res + size_int;
                args_int_compteur ++;
            }
            else if(format[i+1] == 'c')
            {
                args_char[args_char_compteur] = va_arg(args, int);
                args_char_compteur++;
            }
            else if(format[i+1] == 's')
            {
                args_char_ptr[args_char_ptr_compteur] = va_arg(args, char*);
                size_res = size_res + strlen(args_char_ptr[args_char_ptr_compteur])-1;
                args_char_ptr_compteur++;
            }
        }
        size_res ++;
    }
    // Avec la taille on initialise le resultat
    char * result = malloc(size_res+1);
    // on reset les compteur
    args_int_compteur = 0;
    args_char_compteur = 0;
    args_char_ptr_compteur = 0;
    // j permet de garder notre position dans result
    int j = 0;
    // On ecrit dans result tous les caractere et arguments de format dans result
    for(int i = 0; i <= size_format; i++)
    {
        if(format[i] == '%')
        {
            if(format[i+1] == 'd')
            {
                char * int_to_char = malloc(max_size_int);
                sprintf(int_to_char, "%d", args_int[args_int_compteur]);
                args_int_compteur++;
                int k = 0;
                while(int_to_char[k] != '\0')
                {
                    result[j] = int_to_char[k];
                    j++;
                    k++;
                }
                i++;
            }
            else if(format[i+1] == 'c')
            {
                result[j] = args_char[args_char_compteur];
                args_char_compteur++;
                j++;
                i++;
            }
            else if(format[i+1] == 's')
            {
                int k = 0;
                while(args_char_ptr[args_char_ptr_compteur][k] != '\0')
                {
                    result[j] = args_char_ptr[args_char_ptr_compteur][k];
                    j++;
                    k++;
                }
                args_char_ptr_compteur++;
                i++;
            }
            else
            {
                result[j] = format[i];
                j++;
            }
        }
        else
        {
            result[j] = format[i];
            j++;
        }
    }
    // On ferme la va_list
    va_end(args);
    // Enfin on ecrit avec la fonction result dans f1, c'est la fonction ecrire qui choisit si on stocke result dans le buffer ou pas.
    return ecrire(result, 1, size_res, f);
}

void init()
{
    // On alloue et on intialise les variable
    stdout_io = malloc(sizeof(FICHIER));

    stdout_io->size = BUFFER_SIZE;
    stdout_io->buffer = malloc(stdout_io->size);
    stdout_io->cursor = stdout_io->buffer;
    stdout_io->mode = 'E';
    stdout_io->load_data = stdout_io->buffer;
    stdout_io->fd = 1;

    stderr_io = malloc(sizeof(FICHIER));
    stderr_io->size = BUFFER_SIZE;
    stderr_io->buffer = malloc(stderr_io->size);
    stderr_io->cursor = stderr_io->buffer;
    stderr_io->mode = 'E';
    stderr_io->load_data = stderr_io->buffer;
    stderr_io->fd = 2;

}

int ecriref (const char *format, ...)
{
    init();

    // On commence par calculer la taille en caratere du plus grand int
    int int_max = INT_MAX;
    int max_size_int = 0;
    while(int_max >= 10)
    {
        int_max = int_max/10;
        max_size_int ++;
    }
    // Pour le signe - dans le cas ou le nombre est negatif
    max_size_int++;
    // On initialise un va_list
    va_list args;
    va_start(args, format);
    // size_res est la taille en octets (char) de notre chaine resultante du format
    int size_res = 0;
    // On recupere la taille de format
    int size_format = strlen(format);

    // args_int va contenir tous les argument de type int du format dans l'ordre
    int* args_int = malloc(sizeof(int) * (size_format/2));
    // args_int_compteur permet de se souvenir ou on se trouve dans args_int
    int args_int_compteur = 0;
    // args_char va contenir tous les argument de type char du format dans l'ordre
    char* args_char = malloc(sizeof(char) * (size_format/2));
    // args_char_compteur permet de se souvenir ou on se trouve dans args_char
    int args_char_compteur = 0;
    // args_char_ptr va contenir tous les argument de type char* du format dans l'ordre
    char** args_char_ptr = malloc(sizeof(char*) * (size_format/2));
    // args_char_ptr_compteur permet de se souvenir ou on se trouve dans args_char_ptr
    int args_char_ptr_compteur = 0;

    // On recupere ttous les arguments et on calcul la somme de la taille de tous les argument plus les caractere fixe de format
    for(int i = 0; i < size_format; i++)
    {
        if(format[i] == '%')
        {
            if(format[i+1] == 'd')
            {
                args_int[args_int_compteur] = va_arg(args, int);
                int tmp = args_int[args_int_compteur];
                int size_int = 0;
                //On calcul la taille en caractere de l'argument de type int
                if(tmp<0)
                { 
                    size_int ++; 
                    tmp = tmp*-1;
                }
                while(tmp >= 10)
                {
                    tmp = tmp/10;
                    size_int ++;
                }
                size_res = size_res + size_int;
                args_int_compteur ++;
            }
            else if(format[i+1] == 'c')
            {
                args_char[args_char_compteur] = va_arg(args, int);
                args_char_compteur++;
            }
            else if(format[i+1] == 's')
            {
                args_char_ptr[args_char_ptr_compteur] = va_arg(args, char*);
                size_res = size_res + strlen(args_char_ptr[args_char_ptr_compteur])-1;
                args_char_ptr_compteur++;
            }
        }
        size_res ++;
    }
    // Avec la taille on initialise le resultat
    char * result = malloc(size_res+1);
    // on reset les compteur
    args_int_compteur = 0;
    args_char_compteur = 0;
    args_char_ptr_compteur = 0;
    // j permet de garder notre position dans result
    int j = 0;
    // On ecrit dans result tous les caractere et arguments de format dans result
    for(int i = 0; i <= size_format; i++)
    {
        if(format[i] == '%')
        {
            if(format[i+1] == 'd')
            {
                char * int_to_char = malloc(max_size_int);
                sprintf(int_to_char, "%d", args_int[args_int_compteur]);
                args_int_compteur++;
                int k = 0;
                while(int_to_char[k] != '\0')
                {
                    result[j] = int_to_char[k];
                    j++;
                    k++;
                }
                i++;
            }
            else if(format[i+1] == 'c')
            {
                result[j] = args_char[args_char_compteur];
                args_char_compteur++;
                j++;
                i++;
            }
            else if(format[i+1] == 's')
            {
                int k = 0;
                while(args_char_ptr[args_char_ptr_compteur][k] != '\0')
                {
                    result[j] = args_char_ptr[args_char_ptr_compteur][k];
                    j++;
                    k++;
                }
                args_char_ptr_compteur++;
                i++;
            }
            else
            {
                result[j] = format[i];
                j++;
            }
        }
        else
        {
            result[j] = format[i];
            j++;
        }
    }
    // On ferme la va_list
    va_end(args);
    // Enfin on ecrit avec la fonction result dans f1, c'est la fonction ecrire qui choisit si on stocke result dans le buffer ou pas.
    int return_value = ecrire(result, 1, size_res, stdout_io);
    vider(stdout_io);
    return return_value;
}
