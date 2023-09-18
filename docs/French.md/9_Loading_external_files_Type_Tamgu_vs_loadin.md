## Chargement de fichiers externes : Type Tamgu vs Loadin

Le type `tamgu` est utilisé pour charger dynamiquement un programme Tamgu spécifique. D'autre part, `loadin` est utilisé pour charger le contenu d'un fichier dans le programme actuel.

### 9.1 Méthodes

1. `tamgu var(string Tamgu pathname)`: Crée et charge un programme Tamgu.

### 9.2 Exécution de fonctions externes

Les fonctions disponibles dans le fichier Tamgu peuvent être appelées via une variable `tamgu`.

#### Exemple

```cpp
// Dans notre programme test.tmg, nous implémentons la fonction : Read
// Dans test.Tamgu
function Read(string s) {
    return (s + "_toto");
}
//-------------------------------------------
// Dans call.Tamgu
// Dans notre programme appelant, nous chargeons d'abord test.Tamgu, puis nous exécutons Read
tamgu kf('c:\test.tmg'); // Nous chargeons un programme implémentant Read
string s = kf.Read("xxx"); // Nous pouvons exécuter Read dans notre programme local.
```

### Fonctions privées

Si vous ne souhaitez pas que des programmes externes accèdent à certaines fonctions spécifiques, vous pouvez les protéger en déclarant ces fonctions comme privées.

#### Exemple

```cpp
// Nous implémentons une fonction, qui ne peut pas être appelée depuis l'extérieur
private function CannotBeCalled(string s) { ... }
```

### `loadin(string pathname)`

Tamgu offre également une autre façon de charger des fichiers dans un autre programme. `loadin(pathname)` charge le contenu d'un fichier dans le programme actuel. En d'autres termes, `loadin` fonctionne exactement comme si le fichier actuel contenait le code de ce qui est chargé avec `loadin`.

Le code est en fait chargé à l'endroit même où l'appel à `loadin` est écrit.

#### Exemple

```cpp
// Dans notre programme test.tmg, nous implémentons la fonction : Read
// Dans test.Tamgu
function Read(string s) {
    return (s + "_toto");
}
//-------------------------------------------
// Dans call.Tamgu
// Dans notre programme appelant, nous chargeons d'abord test.Tamgu, puis nous exécutons Read
loadin('c:\test.tmg'); // Nous chargeons le code implémentant Read
string s = Read("xxx"); // Nous pouvons exécuter Read directement dans notre programme local.
```