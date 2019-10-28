GHYS Cyril

Tout à été fait et fonctionne correctement.

Catmull-Clark était assez compliqué :
- Je n'avais pas tout de suite pensé à vérifier la direction des arêtes pour éviter de récupérer la même face plusieurs fois.
- J'ai passé beaucouuup de temps à comprendre d'où venait ma dernière erreur. Ce n'est que plus tard que j'ai compris que n n'était pas égal au nombre total de points mais au nombre d'arêtes partant d'un point p. (Peut-être que le PDF du cours mériterait un peu plus de détails).