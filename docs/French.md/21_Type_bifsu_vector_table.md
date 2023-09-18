## Conteneurs Vector et Table de Tamgu

### Conteneurs Vector
Tamgu propose des conteneurs vectoriels spécialisés pour différents types de données. Ces conteneurs sont efficaces en termes de consommation de mémoire et permettent un accès rapide aux éléments. Voici les types de vecteurs disponibles :

- **bvector** : Un conteneur vectoriel pour les octets.
- **ivector** : Un conteneur vectoriel pour les entiers.
- **lvector** : Un conteneur vectoriel pour les longs.
- **fvector** : Un conteneur vectoriel pour les flottants.
- **svector** : Un conteneur vectoriel pour les chaînes de caractères.
- **uvector** : Un conteneur vectoriel pour les chaînes de caractères unicode.

Ces conteneurs ne peuvent stocker que des valeurs de leur type spécifique. Lorsque vous stockez une chaîne de caractères dans un vecteur, Tamgu doit créer un objet de chaîne de caractères, qui sera stocké dans le vecteur car un vecteur ne peut stocker que des objets. Cependant, dans le cas d'un svector, la chaîne de caractères est stockée directement sans avoir besoin d'un objet de chaîne de caractères spécifique. Cela réduit le stockage aux seules chaînes de caractères et améliore la vitesse d'accès.

Pour utiliser ces structures, vous pouvez les traiter exactement comme un vecteur normal. Par exemple :

```cpp
svector test;
test.push("toto");
```

### Conteneur Table
La "table" est un autre conteneur fourni par Tamgu. Il s'agit d'un conteneur de taille fixe qui attend des entiers en tant qu'index. La taille de la table doit être définie lors de sa création et ne peut pas être modifiée par la suite. Le conteneur de table est extrêmement rapide car il est basé sur une implémentation de table en langage C.

Pour créer une table avec une taille spécifique et accéder à ses éléments, vous pouvez faire ce qui suit :

```cpp
table test(10);
test[1] = "i";
```

Si la taille initiale de la table est trop petite, vous pouvez utiliser la méthode "resize" pour agrandir ou réduire la taille. Cette méthode modifiera la taille actuelle de la table et copiera tous les éléments précédents à leurs nouvelles positions. Cependant, si vous réduisez la taille de la table, les éléments au-delà de la nouvelle limite seront perdus.

```cpp
table test(10);
println(test.size()); // Sortie : 10
test.resize(20);
println(test.size()); // Sortie : 20
```

Il est important de noter que le conteneur de table n'est pas protégé contre les opérations de lecture/écriture simultanées dans les threads. Si vous pouvez garantir qu'aucune lecture/écriture simultanée ne se produira sur les mêmes éléments, cette structure peut être très efficace à utiliser car elle réduit le nombre de verrous internes. Cependant, si vous prévoyez des collisions potentielles, il est plus sûr d'utiliser des verrous pour éviter les plantages.

De plus, Tamgu empêche le redimensionnement dans un thread car l'accès concurrentiel aux éléments peut être perturbé.