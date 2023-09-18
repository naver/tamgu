# pytamgu

La bibliothèque `pytamgu` est une bibliothèque double qui peut être utilisée à la fois comme une bibliothèque Python et comme une bibliothèque Tamgu. Elle vous permet d'exécuter du code Python à partir d'un programme Tamgu ou d'exécuter un programme Tamgu à partir du code Python.

## En tant que bibliothèque Tamgu

Lorsqu'elle est utilisée comme une bibliothèque Tamgu, `pytamgu` expose un nouveau type appelé `python`. Les types de base de Tamgu (booléen, entier, long, flottant, fraction, chaîne de caractères, conteneurs de vecteurs et conteneurs de map) sont automatiquement mappés sur les types Python correspondants.

Le type `python` expose les méthodes suivantes :

1. `close()`: Ferme la session Python en cours.
2. `execute(string funcname, p1, p2...)`: Exécute une fonction Python avec `p1`, `p2`, etc. comme paramètres.
3. `import(string python)`: Importe un fichier Python.
4. `run(string code)`: Exécute du code Python.
5. `setpath(string path1, string path2, etc...)`: Ajoute des chemins système à Python.

La méthode `setpath` est essentielle pour utiliser la méthode `import`, qui fonctionne de la même manière que le mot-clé `import` en Python. Si vous souhaitez importer un programme Python à un emplacement spécifique qui n'a pas été référencé via `PYTHONPATH`, vous devez l'ajouter avec `setpath` en premier.

### Exemple

Tout d'abord, nous implémentons un petit programme Python appelé `testpy.py` :

```python
val = "ici"
# Les variables d'entrée sont automatiquement traduites des variables Tamgu en variables Python
def Iteste(s, v):
    v.append(s)
    v.append(val)
    return v
```

Ensuite, nous implémentons notre propre programme Tamgu, qui appellera ce fichier (en supposant qu'il se trouve dans le même répertoire que notre programme Tamgu) :

```tamgu
// Nous devons utiliser Pytamgu pour notre propre intérêt
use("pytamgu");

// Nous avons besoin d'une variable pour gérer le traitement Python
python p;

// Nous supposons que notre programme Python se trouve dans le même répertoire que notre programme Tamgu
p.setpath(_paths[1]);

// Nous importons ensuite notre programme
p.import("testpy");

vector v;
string s = "kkk";

// Nous exécutons la fonction Python Itest, qui prend en entrée une chaîne de caractères et un vecteur,
// qui seront convertis en objets Python à la volée.
// La sortie est automatiquement reconvertie en un vecteur Tamgu (à partir du vecteur Python)
vector vv = p.execute("Itest", s, v);

println(vv); // La sortie est : ['kkk', 'ici']

p.close(); // Nous fermons la session
```

## En tant que bibliothèque Python

Lorsqu'elle est utilisée comme une bibliothèque Python, vous pouvez importer la bibliothèque `pytamgu`, qui expose deux méthodes.

1. `load(file, arguments, mapping)`
   - `file` est le nom de fichier du fichier Tamgu à charger.
   - `arguments` est une chaîne de caractères qui fournit des arguments au fichier Tamgu séparés par un espace.
   - Si `mapping` est défini à 1, une méthode Python est créée pour chaque fonction du fichier Tamgu avec le même nom que les fonctions Tamgu.
   - Cette méthode renvoie une poignée.

2. `execute(handle, function_name, [arg1, arg2, ..., argn])`
   - `handle` est la poignée du fichier qui contient la fonction que nous voulons exécuter.
   - `function_name` est le nom de la fonction dans le fichier Tamgu.
   - `[arg1, arg2, ..., argn]` est la liste des arguments qui seront fournis au programme Tamgu sous la forme d'un vecteur de chaînes de caractères.

Si vous utilisez l'option de mappage, la méthode `execute` est facultative. Les valeurs renvoyées par le programme Tamgu sont automatiquement traduites en objets Python, et il en va de même pour les arguments transmis au programme Tamgu.

**Remarque :** Tamgu renvoie toujours des chaînes de caractères Unicode en Python.

### Exemple

Programme Tamgu :

```tamgu
vector v = [1..10];

function rappel(string s, int j) {
    j += 10;
    v.push(j);
    v.push(s);
    return v;
}
```

Programme Python :

```python
import pytamgu

h0 = pytamgu.load("rappel.kif", "", 1)
# Nous utilisons le mappage vers une fonction Python
v = rappel("Test", 10)
for i in v:
    print(i)
# Cela équivaut à
v = pytamgu.execute(h0, "rappel", ["Test", 10])
```