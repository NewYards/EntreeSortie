# EntreeSortie

Pour tester avec test.c:  
-compiler stdes.c avec test.c  
&emsp;&emsp;gcc test.c stdes.c -o a.out     
-Lancer l'executable  
&emsp;&emsp;./a.out Lecture.txt Ecriture.txt;  
-La sortie doit être :  
le texte de Lecture.txt est ecris en entier dans Ecriture.txt  
Ce test a fonctionné avec notre lib.  
  
Pour tester avec test-rand.c:  
-compiler stdes.c avec test-rand.c  
&emsp;&emsp;gcc test-rand.c stdes.c -o a.out     
-Lancer l'executable  
&emsp;&emsp;./a.out Lecture.txt Ecriture.txt;  
-La sortie doit être :  
Un texte aléatoire.  
Notre lib ecris que chaque caractere est une erreur cependant le fichier rand-file.txt est bien generer avec une suite de caractere aleatoire.  
  
Pour tester avec test-format.c:  
-compiler stdes.c avec test-format.c  
&emsp;&emsp;gcc test-format.c stdes.c -o a.out     
-Lancer l'executable  
&emsp;&emsp;./a.out Lecture.txt Ecriture.txt;  
On ne peut pas connaitre la sortie attendu car notre lib ne contient pas fliref.  

Pour tester avec eval-syscall.c:  
-compiler stdes.c avec eval-syscall.c  
&emsp;&emsp;gcc eval-syscall.c stdes.c -o a.out     
-Lancer l'executable  
&emsp;&emsp;./a.out Lecture.txt Ecriture.txt;  
-sortie attendu:  
Le fichier Ecriture.txt soit equivalent a Lecture.txt  
Avec notre lib le test fonctionne.  

Pour tester avec eval-libc.c:  
-compiler stdes.c avec eval-libc.c  
&emsp;&emsp;gcc eval-libc.c stdes.c -o a.out     
-Lancer l'executable  
&emsp;&emsp;./a.out Lecture.txt Ecriture.txt;  
-sortie attendu:  
Le fichier Ecriture.txt soit equivalent a Lecture.txt  
Avec notre lib le test fonctionne.  
