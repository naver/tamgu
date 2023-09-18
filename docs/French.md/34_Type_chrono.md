# Type Chrono

Le type `Chrono` est similaire à `Time`, mais il est implémenté en se basant sur `std::chrono::high_resolution_clock`. Il fournit la même méthode `reset`, mais permet un calcul du temps plus précis.

Lors de la création d'un objet `Chrono`, vous devez spécifier comment la durée doit être gérée en utilisant les indicateurs suivants :
- `c_second`
- `c_millisecond`
- `c_microsecond`
- `c_nanosecond`

La valeur par défaut est `c_second`.

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