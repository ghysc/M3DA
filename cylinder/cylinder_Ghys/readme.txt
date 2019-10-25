GHYS Cyril

Fichier joint (dans le dossier snapshot) : explicationRevolution.jpg

Tout a été fait et fonctionne parfaitement... bien que j'ai modifié un peu du squelette de GLApplication (pour la révolution) :
Je l'ai modifié pour permettre de correctement joindre les extrémités.
En effet, avant que je ne modifie le squelette, lorsque l'on dessinait avec un _path linéaire, les extrémités se démarquaient
(voir le snapshot n°5 pour lequel les extrémités du donut se chevauchaient ou le n°7 pour lequel elles s'écartaient).
Je pense que cela était dû au fait que le premier et dernier points se chevauchaient. 
J'ai alors pensé qu'il fallait écrire une fonction tangentPathCircle (comme ci-dessous) et de modifier le squelette pour l'appeler
(dans le cas d'un objet dessiné en révolution).

Vector3 GLApplication::tangentPathCircle(unsigned int i) {
  Vector3 result;

  if(i==0)
      result = _path[1]-_path[_path.size()-2]; // au lieu de _path[1]-_path[0];
  else if(i==_path.size()-1)
      result = _path[1]-_path[_path.size()-2]; // au lieu de _path[_path.size()-1]-_path[_path.size()-2];
  else
      result = _path[i+1]-_path[i-1];

  return result;
}

A noter que, si vous le testez, si vous cliquez sur "Switch Linear/Spline Path", il vous faudra recliquer sur "Draw revolution".
(Il s'agissait ici de prouver que mes fonctions fonctionnaient, ce pourquoi je n'ai pas modifié le squelette en profondeur,
veuillez m'en excuser). 

S'il y avait une méthode pour réussir cette question sans modifier le squelette, veuillez m'en excuser, je n'ai pas su la trouver.
Il ne me semblait pas possible de pouvoir m'en sortir avec la précédente fonction tangentPathLine alors que deux points se chevauchaient.
Pour comprendre mon schéma de pensée, veuillez retrouver mon schéma explicationRevolution.jpg joint dans le dossier snapshot.
