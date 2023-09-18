# Types sans verrou

Les types sans verrou sont extrêmement utiles dans les applications multithread car ils permettent la manipulation de nombres, de chaînes de caractères, de vecteurs ou de maps sans avoir besoin de verrous. Les verrous peuvent avoir un impact significatif sur les performances d'exécution. Lorsqu'un thread est mis en attente en raison d'un verrou, le système doit stocker son contexte et le réactiver une fois que le verrou est libéré. La programmation sans verrou vise à minimiser l'utilisation des verrous afin d'éviter des interruptions coûteuses dans le flux d'exécution.

## Table et Rawstring

Dans les sections précédentes, nous avons discuté de deux objets sans verrou : table et rawstring.

Ces objets ont la caractéristique unique que leur taille est fixe et ne peut pas être modifiée dans le contexte d'un thread. Cela garantit que l'accès concurrent à ces objets n'interférera pas avec leur déclaration de tampon interne.

## Valeurs atomiques : a_bool, a_int, a_float, a_string, a_ustring

Tamgu propose également cinq valeurs atomiques pour les booléens, les entiers, les flottants et les chaînes de caractères. Ces objets peuvent être utilisés comme des valeurs normales, mais en cas d'accès concurrent, leur atomicité est garantie. L'atomicité, dans ce contexte, signifie qu'un seul thread peut modifier leur valeur à la fois.

## Conteneurs : a_vector, a_[i|f|s|u]vector, a_map [i|f|s|u][i|f|s|u]

Tamgu propose également des conteneurs qui peuvent être modifiés par différents threads simultanément avec une utilisation minimale de verrous. Ces structures peuvent être utilisées exactement comme leurs maps non atomiques correspondantes. Elles sont implémentées comme une liste chaînée de morceaux, chaque morceau pouvant accueillir un nombre significatif de valeurs. Les verrous ne sont utilisés que lorsqu'un nouvel élément est ajouté à la liste chaînée. Les opérations de lecture et la plupart des opérations de stockage sont effectuées sans aucun verrou, sauf lorsque la map ou le vecteur n'est pas suffisamment grand pour accueillir un nouvel élément.

Note : les a_maps sont des conteneurs de valeurs, tandis que a_vector est un conteneur d'objets.

Note : Vous devez charger la bibliothèque "allmaps" pour accéder à ces maps.

## Ring

"Ring" est un autre conteneur sans verrou qui peut contenir un maximum de 65535 éléments. Il est implémenté comme une structure où les éléments peuvent être stockés ou supprimés à partir de l'avant ou de l'arrière. Contrairement aux autres conteneurs, ces opérations ont le même temps de mémoire ou la même empreinte spatiale.

## Important

Lorsque plusieurs threads tentent de modifier la même variable sans verrou, il n'y a aucune garantie que les deux modifications seront réussies. Le dernier thread à accéder à la variable peut être celui qui modifie sa valeur.