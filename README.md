# EntreeSortie

Pour tester lire:  
-compiler stdes.c  
&emsp;&emsp;gcc stdes.c;  
-Lancer l'executable  
&emsp;&emsp;./a.out Lecture.txt Ecriture.txt;  
-La sortie attendu est :  
Lecture d'un fichier ouvert en ecriture refusé.  
&emsp;&emsp;Cas buffer vide et size_wanted < f->size  
&emsp;&emsp;&emsp;&emsp;    Taille lus : 3  
&emsp;&emsp;&emsp;&emsp;    Octets lus : Lec  
&emsp;&emsp;Cas buffer vide et size_wanted > f->size  
&emsp;&emsp;&emsp;&emsp;    Taille lus : 7  
&emsp;&emsp;&emsp;&emsp;    Octets lus : Lecture  
&emsp;&emsp;Cas buffer non vide et size_wanted < f->size  
&emsp;&emsp;&emsp;&emsp;    Taille lus : 1  
&emsp;&emsp;&emsp;&emsp;    Octets lus : L  
&emsp;&emsp;&emsp;&emsp;    Taille lus : 1  
&emsp;&emsp;&emsp;&emsp;    Octets lus : e  
&emsp;&emsp;Cas buffer non vide et size_wanted > f->size  
&emsp;&emsp;&emsp;&emsp;    Taille lus : 1  
&emsp;&emsp;&emsp;&emsp;    Octets lus : L  
&emsp;&emsp;&emsp;&emsp;   Taille lus : 6  
&emsp;&emsp;&emsp;&emsp;    Octets lus : ecture  
&emsp;&emsp;Cas buffer non vide et size_wanted > f->size  
&emsp;&emsp;&emsp;&emsp;    Taille lus : 0  
&emsp;&emsp;&emsp;&emsp;    Octets lus :  
  
