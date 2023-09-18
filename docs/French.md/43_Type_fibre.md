# 43 Type Fibre

Une fibre est un fil d'exécution léger. Comme les threads, les fibres partagent le même espace d'adressage. Cependant, les fibres utilisent une multitâche coopérative tandis que les threads utilisent une multitâche préemptive. Les threads s'appuient souvent sur le planificateur de threads du noyau pour préempter un thread occupé et reprendre l'exécution d'un autre thread, tandis que les fibres se laissent passer la main pour permettre à une autre fibre de s'exécuter.

Dans Tamgu, les fibres sont implémentées au-dessus des fonctions Taskell. Ces fonctions doivent contenir une itération pour être exécutées en tant que fibres, car il n'y a pas de fonction de mise en attente réelle.

Le type `fibre` expose les méthodes suivantes :

1. `run()`: Exécute les fibres qui ont été enregistrées.
2. `block()`: Les fibres sont stockées dans une chaîne liée qui est itérée du début à la fin. Lorsqu'une nouvelle fibre est ajoutée à cette liste, elle devient le nouvel élément terminal. `block()` est utilisé pour définir l'élément terminal actuel de cette liste comme limite d'itération. De nouvelles fibres peuvent encore être ajoutées, mais elles ne seront pas exécutées tant que `unblock()` n'est pas appelé. Si `block()` est appelé à nouveau avec un nouvel élément terminal actuel différent, la limite précédente est déplacée vers ce nouvel élément terminal.
3. `unblock()`: Libère la limite d'itération.

## Déclaration d'une fibre

Pour déclarer une fibre, vous devez d'abord implémenter une fonction Taskell, puis stocker cette fonction Taskell dans une variable fibre.

```cpp
<Myfunc(a1, a2, ..., an) = ...>
fibre f = MyFunc;
```

Pour enregistrer une nouvelle fibre, il suffit de l'appeler avec certains paramètres :

```cpp
f(p1, p2, ..., pn);
```

Si `MyFunc` ne contient pas d'itération, il sera automatiquement exécuté. Si vous avez besoin de stocker une sortie, vous pouvez utiliser l'opérateur de flux.

```cpp
vector v;
v.push(ivector());
v[-1] <<< f(200, 210);
```

## Fin d'une fibre : `break` ou fin de l'itération

Il existe deux façons de mettre fin à une fibre : soit l'itérateur atteint une fin, soit l'instruction `break` est appelée.

```cpp
<Myfunc(x, y) : if (x == 10) break else (x + 1) ...>
```

## Threads

Les fibres peuvent être exécutées dans un thread, mais une seule fibre peut exécuter la méthode `run()` à la fois dans un thread. Cependant, les threads peuvent enregistrer de nouvelles fibres en parallèle sans aucun problème.

## Exemple

```cpp
vector v;
int i;

// Nous initialisons un vecteur d'entiers pour stocker les résultats...
for (i in <3>)
    v.push(ivector());

//-----------------------------------------------------------------------
function compute(int i, int x, int y) {
    return i + x + y;
}

<myfiber(x, y) = compute(i, x, y) | i <- [x..y]>

// Nous initialisons une variable fibre avec myfiber
fibre f = myfiber;

//-----------------------------------------------------------------------
// L'enregistrement est effectué à partir d'un thread...
joined thread recording(int i, int x, int y) {
    println("Fibre:", i);
    v[i] <<< f(x, y);
}

function running() {
    f.run();
}

//-----------------------------------------------------------------------
// Les fibres sont lancées à partir d'un thread...
int n;

for (i in <0, 60, 20>) {
    recording(n, i, i + 19);
    n++;
}

// Nous attendons que toutes les fibres soient enregistrées
waitonjoined();

//-----------------------------------------------------------------------
// Nous les exécutons...
running();

println(v);
```

**Résultat :**

```
Fibre: 0
Fibre: 2
Fibre: 1
[[19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38],
[79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98],
[139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158]]
```