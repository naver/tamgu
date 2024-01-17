# Types: chrono et clock

Le type `chrono` est similaire à `time`, mais il est implémenté en se basant sur `std::chrono::high_resolution_clock`. Il fournit la même méthode `reset`, mais permet un calcul du temps plus précis.

Lors de la création d'un objet `Chrono`, vous devez spécifier comment la durée doit être gérée en utilisant les indicateurs suivants :
- `c_second`
- `c_millisecond`
- `c_microsecond`
- `c_nanosecond`

La valeur par défaut est `c_second`.

## Méthodes

1. `reset()`:  Réinitialise l'horloge.
2. `start()`: Démarre le chrono.
3. `stop()`: Arrête le chrono et retourne l'heure intermédiaire
4. `unit(int i)`: 1 pour _seconde_,  2 pour _milliseconds_, 3 pour _microsecond_, 4 pour _nanosecond_.


## Exemple

```cpp
Ring r;
Chrono c1(c_nanosecond);
for (int i = 0; i < 100000; i++) {
    r.pushLast(i);
}
Chrono c2(c_nanosecond);
float d;
d = c2 - c1; // en nanosecondes
println(d);
```

Dans cet exemple, un objet `Ring` `r` est créé, et un objet `Chrono` `c1` est créé avec une durée de `c_nanosecond`. Ensuite, une boucle est exécutée pour ajouter des valeurs à l'anneau. Après cela, un autre objet `Chrono` `c2` est créé avec la même durée. La différence entre `c2` et `c1` est calculée et stockée dans `d`, représentant le temps écoulé en nanosecondes. Enfin, la valeur de `d` est affichée.

# Type clock

Le type `clock` est similaire à `chrono`, mais il est implémenté en se basant sur `std::chrono::system_clock`. Ce type en particulier fournit en particulier des méthodes supplémentaires pour gérer le temps UTC.

## Méthodes

On rajoute aux méthodes de _chrono_:

1. `utc()`: Renvoie l'horloge au format UTC
2. `utc(string fmt)`: Initialise l'horloge à partir d'une chaine au format UTC
3. `timezone(string tm)`: Initialise le fuseau horaire avec une chaine de type: "+|-dd:dd"
4. `format(string fmt)`: Renvoie une chaine au format fmt. On rajoute aussi %fm comme balise supplémentaire pour renvoyer les millisecondes et `%fc`pour renvoyer les microsecondes.
5. `microseconds()` : Renvoie les microseconds

## Initialisation

Lorsque l'on crée un objet _clock_, on peut aussi lui donner comme argument le fuseau horaire sous la forme d'une chaine de caractères: "+|-dd:dd".

### Exemple

```cpp
clock d("-05:30");
```


