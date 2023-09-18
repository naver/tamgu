# Opérateur in

Cet opérateur a un comportement riche, c'est pourquoi nous avons une section dédiée pour l'expliquer. Dans la description précédente, nous avons déjà discuté de quelques utilisations possibles de cet opérateur avec des fichiers, des vecteurs, des maps ou des chaînes de caractères. Maintenant, voyons comment il peut également être utilisé avec des frames.

## 1. "in" dans un frame

Un frame peut avoir une fonction `in`, qui est utilisée lorsque l'opérateur `in` est appliqué à un frame. Si un objet frame n'a pas de fonction `in`, alors l'opérateur retourne toujours false.

### Exemple

```cpp
// Ceci est un exemple d'utilisation de l'opérateur in avec une map.
map dico;
vector lst;
dico = { 'a': 1, 'b': 6, 'c': 4, 'd': 6 };

// Test booléen, il retourne true ou false
if (6 in dico)
    print("Comme prévu\n");

// Le récepteur est une liste, donc nous retournons la liste des indexes
lst = 6 in dico;
string s;
for (s in lst)
    print("LST: ", s, "\n");
```

Sortie :
```
Comme prévu
LST: b
LST: d
```

Comme nous pouvons le voir dans cet exemple, le système retourne des informations liées au type du récepteur.

### Exemple avec un frame

```cpp
frame testframe {
    int i;

    // Le type du paramètre peut être n'importe quoi
    function in(int j) {
        if (i == j)
            return true;
        return false;
    }
}
```

Veuillez noter que le code a été formaté pour une meilleure lisibilité.