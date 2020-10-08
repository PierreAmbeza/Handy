# Handy
Jeu de pierre/feuille/ciseaux réalisé avec OpenCV réalisé dans le cadre d'un projet scolaire.

## Principe

Il s'agit ici de trouver une méthode permettant de décider du symbole choisi par l'utilisateur en fonction de la forme de sa main. 5 doigts levés et face à la caméra correspond à la feuille, deux doigts levés au ciseaux et le poing (face à la caméra) à la pierre. La première étape est de détecter la main de l'utilisateur, ou du moins, le contour de celle-ci. Pour cela, une conversion en HSV de l'image acquise par la webcam de l'ordinateur est réalisée puis un seuillage **Hat** est appliqué à l'aide de trackbars afin d'isoler la main. Enfin, une dilatation est effectuée pour combler les éventuels trous qui n'auraient pas lieu d'être au sein du contour de la main. La deuxième étape est désormais d'identifier le symbole choisi par l'utilisateur. Pour cela, nous utilisons l'enveloppe convexe du contour de la main et les défauts de convexité. Ensuite, nous conservons uniquement les défauts dont le point le plus profond dépasse une certaine profondeur pré-établie, défauts qui correspondent donc au creux des doigts. Pour finir, nous comptons le nombre de défauts conservés pour décider du symbole choisi par l'utilisateur. Il ne reste alors plus qu'à décider d'un symbole aléatoire comme choix de l'ordinateur.
**Autres éléments:** Le jeu comprend un affichage des scores et l'affichage du choix de l'ordinateur par une image correspondant au symbole choisi aléatoirement.




