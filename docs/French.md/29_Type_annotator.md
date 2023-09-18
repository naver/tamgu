# Annotateur

## Description

Un `annotateur` est un outil d'annotation de texte basé sur des règles. Il vous permet de définir des règles pour étiqueter des motifs spécifiques dans un texte. Les règles de l'annotateur sont divisées en quatre groupes : règles de lexique, règles d'annotation, règles globales et règles de suppression.

## Syntaxe des règles

Une règle dans l'outil `annotateur` est une expression régulière qui combine des jetons et des catégories sémantiques. Elle renvoie une étiquette associée à l'étendue des jetons correspondants dans le texte d'origine.

Le corps d'une règle est écrit après le symbole `<-` ou `←`. Les opérateurs disponibles dans une règle sont :

- `{t1,t2,t3...}` : Correspond à l'un des jetons répertoriés.
- `?` : Correspond à n'importe quel jeton.
- `#label` : Correspond à une étiquette sémantique produite par une règle ou une étiquette de lexique.
- `#{l1,l2...}` : Correspond si l'étiquette est l'une des étiquettes répertoriées (disjonction).
- `#[l1,l2...]` : Correspond si l'étiquette correspond à toutes les étiquettes répertoriées (conjonction).
- `*`, `+ (:c)` : Opérateurs de Kleene pouvant être bornés par un compteur.
- `~` : Négation (uniquement des éléments atomiques).
- `(...)` : Séquence facultative de jetons.
- `[...]` : Séquence de jetons.
- `>..<` : Ignore les jetons de cette séquence dans l'annotation finale.
- `<call p1 p2>` : Appelle une fonction qui renvoie vrai ou faux.
- `token` : Correspond à un seul jeton.
- `'rgx'` : Correspond à une expression régulière basée sur la syntaxe des expressions régulières POSIX.
- `"metas"` : Correspond à une expression régulière spécifique à Tamgu basée sur des métacaractères.

## Méthodes de l'annotateur

Le type `annotateur` expose plusieurs méthodes pour étudier les annotations produites par les règles :

- `compile(string rules)` : Compile les règles stockées dans la chaîne `rules` dans la variable `annotateur`. Seule cette variable peut accéder aux règles.
- `select(uvector labels)` : Sélectionne les règles dont les étiquettes sont définies dans `labels`.
- `clear()` : Efface la sélection d'étiquettes.
- `selection()` : Renvoie la sélection d'étiquettes.
- `lexicon(transducer t)` : Associe un lexique de transducteur polyvalent à l'`annotateur`.
- `compilelexicons()` : Précompile les lexiques. S'il n'est pas appelé avant l'analyse, la première analyse les compilera, ce qui peut entraîner des retards.
- `spans(bool)` : Renvoie à la fois les classes d'annotation et leurs décalages si `true`, ou uniquement les classes d'annotation si `false`.
- `parse(ustring txt, bool keeplex)` : Applique les règles à une `ustring`.
- `apply(uvector tokens, bool keeplex)` : Applique les règles à un vecteur de jetons.
- `apply(vector morphos, bool keeplex)` : Applique les règles à un vecteur de jetons analysés morphologiquement.
- `apply(annotator a, bool keeplex)` : Applique les règles à la structure calculée par une analyse ou une application `annotateur` précédente.

## Fonction de rappel

Vous pouvez associer une fonction de rappel à un objet `annotateur`. Cette fonction de rappel est appelée après que l'analyse ait eu lieu mais avant l'application des règles aux textes. La fonction de rappel a la signature suivante :

```cpp
function catching(vector v, annotator a) {
    v = pos.tagger(v);
    return v;
}
```

La fonction `catching` peut être utilisée pour ajouter plus d'analyse aux annotations. Le vecteur `v` est une liste de sous-vecteurs, où chaque sous-vecteur contient le mot, son lemme, ses caractéristiques, son décalage gauche et son décalage droit. La fonction doit renvoyer un vecteur avec la même structure.

## Règles de lexique

Une règle de lexique est utilisée pour décrire le vocabulaire d'un domaine. Elle associe des mots à des étiquettes spécifiques. Les règles de lexique peuvent être des mots simples ou des expressions de plusieurs mots. Les expressions de plusieurs mots ne peuvent être reconnues comme un seul jeton que si elles sont écrites à l'aide d'expressions régulières Tamgu.

## Règles d'annotation

Une règle d'annotation est une étiquette associée à un corps. Le corps peut inclure des éléments facultatifs, une disjonction d'éléments, des expressions régulières, des mots ignorés, etc. Elle renvoie une liste d'étiquettes avec leurs étendues dans le texte d'origine sous forme de décalages.

## Règles globales

Les règles globales commencent par un `#` ou `~` et sont similaires aux règles d'annotation. Cependant, elles ne sont pas appliquées le long du texte mais après que le texte ait été entièrement traité pour gérer les annotations. Les règles globales peuvent ajouter de nouvelles annotations ou supprimer des annotations existantes.

## Exemple

```cpp
// Lexique
@positive  ← great.
@positive  ← better.
@food  ← sushi.

// Règles d'annotation
food ← (#positive), #food +.

// Créer un annotateur
annotator r;

// Compiler les règles
r.compile("@positive  ← great.\n@positive  ← better.\n@food  ← sushi.\nfood ← (#positive), #food +.");

// Appliquer les règles au texte
ustring txt = "The sushi was great.";
r.parse(txt, true);
```

Dans l'exemple ci-dessus, l'`annotateur` est créé, les règles sont compilées, puis appliquées au texte "The sushi was great.". Les annotations résultantes seront stockées dans l'objet `annotateur`.