Ghys Cyril

J'ai renconté un souci lors de l'exercice 3 (pour le cercle), et ai sinon répondu aux questions.


EXERCICE 1

> Question 3: 
1) Selon le degré, la fonction de base s'étendra plus ou moins loin. Son intervalle d'influence s'étend de son point d'indice k jusqu'au point d'indice k+p+1.
2) Pour une valeur u donnée, le nombre de fonctions de base non-nuls (noté o) peut s'élever à p+1 (voir le snapshot000 qui montre qu'avec deux noeuds en plus (soit 8), pour les fonctions de base de degré 3, o s'élève à 4). o croit jusq'à ce que le degré atteigne le nombre de noeuds divisé par 2. Ensuite, le nombre de fonctions de base affichées décroissant, o en fait de même.
3) Avec 6 points de contrôle, il n'y a qu'une seule courbe pour le degré 4 car les fonctions de base dépendent de l'indice du noeud et du degré (Nkp). Ainsi, elle va s'étendre de k jusqu'à k+p+1, comme dit précédemment. Or, s'il n'y a pas assez de points de contrôle, pas possible de calculer la fonction de base.

> Question 4 :
Réponse aux questions/remarques :
1) Voir le snapshot001.
2) Voir le snapshot002.



EXERCICE 2

> Question 2 :
Avec une répartition des noeuds de manière uniforme ouverte, les extrémités semblent + "peser" dans la représentation de la courbe.

> Question 3 :
Le degré d'une courbe de Bézier est égal au nombre de points de contrôle -1. On obtient bien une courbe de Bézier, et nous n'avons plus de contrôle local : on peut toujours bouger les noeuds mais si on le fait, ce n'est plus une courbe de Bézier. Pour notre application, il faut garder à l'esprit qu'on ne peut pas changer le degré avec o et p car celui-ci dépend du nombre de points de contrôle.



EXERCICE 3

> Question 3
Après vous avoir posé la question : p3d(a, b, 0) = p(a*c, b*c, 0, c). Je n'avais simplement pas pensé à multiplier a et b par c... Maintenant, je pense que ça fonctionne bien car l'affichage du cercle en révolution donne bien une sphère (voir le snapshot007)(bien que l'affichage d'un cercle à plat n'en affiche pas un - voir le snapshot008).



EXERCICE 4

> Question 4
Vous pourrez constater que le dernier snapshot006 représente une magnifique lampe IKEA, fait avec des noeuds répartis uniformément.
