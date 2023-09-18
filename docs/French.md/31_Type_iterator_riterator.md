# Itérateur et Ritérateur

Ces itérateurs sont utilisés pour itérer sur des objets de différents types tels que des chaînes de caractères, des vecteurs, des maps et des règles.

Un ritérateur est un itérateur inverse qui permet d'itérer depuis la fin de la collection.

## Méthodes

1. `begin()`: Initialise l'itérateur au début de la collection.
2. `end()`: Renvoie true lorsque la fin de la collection est atteinte.
3. `key()`: Renvoie la clé de l'élément courant.
4. `next()`: Passe à l'élément suivant dans la collection.
5. `value()`: Renvoie la valeur de l'élément courant.

## Initialisation

Un itérateur peut être initialisé par une simple affectation.

## Exemple

```cpp
vector<int> v = {1, 2, 3, 4, 5};
iterator it = v;

for (it.begin(); it.end(); it.next()) {
    print(it.value(), ",");
}
```

## Sortie

```
1, 2, 3, 4, 5,
```