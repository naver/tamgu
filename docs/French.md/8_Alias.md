## Alias

Un alias est similaire à une fonction, mais avec quelques différences spécifiques :

a) Les paramètres sont décrits sans aucun type.
b) Les arguments dans un alias peuvent tous être modifiés.
c) Un alias peut être utilisé pour remplacer un appel de structure complexe.
d) Un alias peut être utilisé comme une fonction simple, auquel cas il doit renvoyer sa valeur avec `return`.

### Modèle

Une fonction alias est implémentée de la manière suivante :

```
alias nom(a, b, ..., z) = code;
```

### Exemple

```python
alias suppression(a) = a.replace("-", "").replace(r"%p", " ").lower();
alias affichage(a, b) = {
    println(a, b);
}

a = suppression(a);
affichage("Test", a);
```

### Modèles complexes

L'exemple ci-dessous montre comment vous pouvez créer une fonction alias qui accède au contenu d'un cadre :

```python
alias obtenir(a, i) = a.personnes.pnoms[i];
```

Cet alias peut être utilisé à la fois pour récupérer un élément ou pour le modifier. Lorsque vous avez besoin de modifier un élément, vous devez utiliser les crochets suivants : `[*...]` pour fournir les différents arguments.

#### Exemple

```python
// Deux descriptions de cadre
cadre ListeDePersonnes {
    svector pnoms;
    fonction _initial() {
        pnoms = ["a".."e"];
    }
    fonction string() {
        return pnoms;
    }
}

cadre Articles {
    ListeDePersonnes personnes;
    fonction string() {
        return personnes;
    }
}

// Nous définissons un alias pour accéder à pnoms dans ListeDePersonnes
alias obtenir(a, i) = a.personnes.pnoms[i];

// Nous déclarons un élément de type Articles.
Articles itms;

// Nous accédons à la valeur de pnoms à la position 1
println(obtenir(itms, 1));

// Nous modifions une valeur de pnoms à la position 1
// Notez l'utilisation de [*itms, 1]
obtenir[*itms, 1] = "B";
```