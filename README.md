# EntreeSortie

Pour tester lire:  
-compiler stdes.c  
&emsp;&emsp;gcc stdes.c;  
-Lancer l'executable  
&emsp;&emsp;./a.out Lecture.txt Ecriture.txt;  
-La sortie attendu est :  
Lecture d'un fichier ouvert en ecriture refusé.  
Cas 1 char  
&emsp;&emsp;    Taille lus : 1  
&emsp;&emsp;    Buffer : A  
Cas buffer vide et size_wanted < f->size  
&emsp;&emsp;    Taille lus : 4  
 &emsp;&emsp;   Buffer : Test  
Cas buffer vide et size_wanted = f->size  
&emsp;&emsp;    Taille lus : 10  
&emsp;&emsp;    Buffer :  
Cas buffer vide et size_wanted = f->size  
&emsp;&emsp;    Taille lus : 12  
&emsp;&emsp;    Buffer :  
Sepration cas buffer vide et cas buffer non vide  
Cas buffer non vide et size_wanted < f->size  
&emsp;&emsp;    Taille lus : 4  
&emsp;&emsp;    Buffer : Test  
&emsp;&emsp;    Taille lus : 4  
&emsp;&emsp;    Buffer : TestTest  
&emsp;&emsp;    Taille lus : 4  
&emsp;&emsp;    Buffer : Test  
Cas buffer non vide et size_wanted = f->size  
&emsp;&emsp;    Taille lus : 4  
&emsp;&emsp;    Buffer : Test  
&emsp;&emsp;    Taille lus : 4  
&emsp;&emsp;    Buffer : TestTest  
&emsp;&emsp;    Taille lus : 10  
&emsp;&emsp;    Buffer :  
Cas buffer non vide et size_wanted > f->size  
&emsp;&emsp;    Taille lus : 4  
&emsp;&emsp;    Buffer : Test  
&emsp;&emsp;    Taille lus : 4  
&emsp;&emsp;    Buffer : TestTest  
&emsp;&emsp;    Taille lus : 12  
&emsp;&emsp;    Buffer :  

  
