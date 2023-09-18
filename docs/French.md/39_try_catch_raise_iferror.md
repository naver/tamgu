# Gestion des erreurs en Python

En Python, il existe plusieurs façons de gérer les erreurs : `try`, `catch`, `raise` et `iferror`. Ces techniques sont utilisées pour gérer et gérer les erreurs dans un programme.

## Instruction Try-Catch

Les instructions `try` et `catch` sont utilisées ensemble pour capturer et gérer les erreurs en Python. L'instruction `catch` peut être associée à une chaîne de caractères ou à un paramètre entier, qui est automatiquement défini sur `null` lorsque le bloc `try` est évalué. Il est également possible d'utiliser une instruction `catch` sans variable.

```python
s = ""
try:
    # code qui peut générer une erreur
    ...
catch (s)
```

Lorsqu'une erreur est détectée, le message d'erreur ou son numéro est transmis à la variable spécifiée.

## La méthode `raise`

La méthode `raise` est utilisée pour générer une erreur avec un message spécifique. Le message d'erreur doit toujours commencer par un numéro d'erreur sur trois caractères, tel que "000". Ce numéro d'erreur doit être supérieur à 200, car les numéros d'erreur inférieurs à 200 sont réservés aux erreurs internes de KF. Cependant, aucune vérification ne sera effectuée par le langage.

```python
raise("201 Mon erreur")
```

## L'instruction `iferror`

L'instruction `iferror` est utilisée pour vérifier si une instruction échoue. Si l'instruction échoue, un bloc de code spécifique est exécuté. Le message d'erreur est alors disponible dans la variable `_iferror`.

```python
txt = ""
iferror(txt.read(path)):
    print("Erreur :", _iferror)
```

## Exemple

Voici un exemple qui illustre l'utilisation des instructions `raise` et `iferror` :

```python
raise("201 Mon erreur")
txt = ""
iferror(txt.read(path)):
    print("Erreur :", _iferror)
```

Dans cet exemple, l'instruction `raise` génère une erreur avec le message "201 Mon erreur". L'instruction `iferror` vérifie si l'instruction `txt.read(path)` échoue. Si c'est le cas, le bloc de code est exécuté et le message d'erreur est affiché.