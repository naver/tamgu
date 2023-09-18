# Extensions

Les extensions vous permettent d'ajouter de nouvelles méthodes à certains types, tels que map, vector, int, string et autres. Le nom de l'extension doit être l'un des types valides suivants : string, automat on, date, time, file, integer, float, vector, list, map, set.

## Définition de l'extension

Pour définir une extension, vous créez un cadre où vous déclarez les nouvelles méthodes que vous souhaitez ajouter au système.

Si vous avez besoin de faire référence à l'élément actuel, vous pouvez utiliser une variable dont le nom est le type lui-même avec "_" comme préfixe. Par exemple, si vous étendez le type "vector", la variable serait "_vector".

Il est important de noter que si vous ajoutez de nouvelles méthodes à "map", toutes les maps hériteront de ces nouvelles méthodes. Il en va de même pour les vecteurs.

### Exemple :

```python
// Nous étendons map pour retourner une valeur, qui sera supprimée de la map.
extension map {
    // Nous ajoutons cette nouvelle méthode, qui sera disponible pour toutes les maps...
    function returnAndDelete(int key) {
        // Nous extrayons la valeur avec notre clé
        string s = _map[key];
        // Nous la supprimons
        _map.pop(key);
        return s;
    }
}

map mx = {1: 2, 3: 4};
// returnAndDelete est maintenant disponible pour tous les types de map.
string s = mx.returnAndDelete(1);

imap imx = {1: 2, 3: 4};
int x = imx.returnAndDelete(1);
```

Dans l'exemple ci-dessus, nous étendons le type "map" pour ajouter une nouvelle méthode appelée "returnAndDelete". Cette méthode retourne une valeur de la map et la supprime. La méthode est ensuite utilisée sur un objet map appelé "mx" et un objet imap appelé "imx".