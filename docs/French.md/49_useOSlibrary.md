## Utilisation de bibliothèques dynamiques dans les programmes Tamgu

La fonction `use(OS, library)` dans Tamgu est utilisée pour charger des bibliothèques dynamiques compatibles dans un programme. Cela permet d'ajouter de nouvelles fonctionnalités telles que des interfaces graphiques et la gestion de bases de données.

Le drapeau `OS` est facultatif et peut prendre l'une des valeurs suivantes : "WINDOWS", "MACOS", "UNIX", "UNIX64". Il est utilisé pour charger des bibliothèques spécifiques en fonction de l'architecture de la plateforme.

La bibliothèque peut être spécifiée soit par son simple nom, soit par son chemin complet. Si vous utilisez le simple nom, il doit correspondre à un nom de bibliothèque stocké dans le répertoire spécifié par la variable d'environnement `TAMGULIBS`.

Les conventions de nommage des bibliothèques varient en fonction de la plateforme :
- Sur les plates-formes Unix, les noms de bibliothèques sont généralement sous la forme `libmonom`yname.so`. Pour charger une telle bibliothèque, il suffit d'appeler `use("myname")`.
- Sur Windows, les noms de bibliothèques sont généralement sous la forme `myname.dll`. Pour charger une telle bibliothèque, il suffit d'appeler `use("myname")`.

Il est généralement recommandé d'utiliser le format de nom simple (`use("myname")`) pour assurer la compatibilité entre les plates-formes. Cependant, si nécessaire, le chemin complet de la bibliothèque peut être spécifié, limitant ainsi l'utilisation du code à des plates-formes spécifiques. Dans de tels cas, le drapeau `OS` peut être utilisé pour réintroduire un certain niveau de généralisation.

Exemple d'utilisation :
```python
use("WINDOWS", "Tamgu sqlite")
```