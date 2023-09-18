# Bibliothèque wapiti

libwapiti est une encapsulation de la bibliothèque wapiti, disponible sur [http://wapiti.limsi.fr](http://wapiti.limsi.fr).

Droits d'auteur (c) 2009-2013 CNRS. Tous droits réservés.

Wapiti offre une implémentation efficace de la méthode CRF pour l'étiquetage et l'extraction d'entités. Si vous avez besoin d'informations sur le système, veuillez vous référer au manuel sur [http://wapiti.limsi.fr/manual.html](http://wapiti.limsi.fr/manual.html).

## Utilisation

Pour utiliser la bibliothèque, vous devez charger le module wapiti :

```python
use('wapiti')
```

La bibliothèque expose les méthodes suivantes :

### Méthodes

1. `loadmodel(string crfmodel)`: Charge un modèle CRF.
2. `options(svector options)`: Définit les options. Voir ci-dessous pour les options disponibles. Les options doivent être placées dans le svector telles qu'elles sont utilisées en ligne de commande avec wapiti.
3. `train()`: Lance l'entraînement. Les options doivent avoir été définies au préalable.
4. `label(vector words)`: Lance l'étiquetage pour un vecteur de jetons. Renvoie un vecteur d'étiquettes pour chaque jeton.
5. `lasterror()`: Renvoie la dernière erreur survenue.

### Options

Wapiti expose certaines options pour traiter toutes les possibilités inhérentes au système. Voici une liste de ces options, qui doivent être fournies sous forme de svector, exactement comme ces options seraient fournies avec la version en ligne de commande de wapiti.

#### Mode d'entraînement :

- `train [options] [input data] [model file]`
  - `--me`: Force le mode maxent
  - `--type STRING`: Type de modèle à entraîner
  - `--algo STRING`: Algorithme d'entraînement à utiliser
  - `--pattern FILE`: Motifs pour l'extraction de caractéristiques
  - `--model FILE`: Fichier de modèle à précharger
  - `--devel FILE`: Jeu de données de développement
  - `--rstate FILE`: État de l'optimiseur à restaurer
  - `--sstate FILE`: État de l'optimiseur à sauvegarder
  - `--compact`: Modèle compact après l'entraînement
  - `--nthread INT`: Nombre de threads de travail
  - `--jobsize INT`: Taille du travail pour les threads de travail
  - `--sparse`: Activer le mode avant/arrière parcimonieux
  - `--maxiter INT`: Nombre maximum d'itérations
  - `--rho1 FLOAT`: Paramètre de pénalité L1
  - `--rho2 FLOAT`: Paramètre de pénalité L2
  - `--objwin INT`: Taille de la fenêtre de convergence
  - `--stopwin INT`: Taille de la fenêtre d'arrêt
  - `--stopeps FLOAT`: Valeur d'epsilon d'arrêt
  - `--clip`: Clipper le gradient (l-bfgs)
  - `--histsz INT`: Taille de l'historique (l-bfgs)
  - `--maxls INT`: Nombre maximal d'itérations de recherche linéaire (l-bfgs)
  - `--eta0 FLOAT`: Taux d'apprentissage (sgd-l1)
  - `--alpha FLOAT`: Paramètre de décroissance exponentielle (sgd-l1)
  - `--kappa FLOAT`: Paramètre de stabilité (bcd)
  - `--stpmin FLOAT`: Taille minimale du pas (rprop)
  - `--stpmax FLOAT`: Taille maximale du pas (rprop)
  - `--stpinc FLOAT`: Facteur d'incrémentation du pas (rprop)
  - `--stpdec FLOAT`: Facteur de décrémentation du pas (rprop)
  - `--cutoff`: Projection alternative (rprop)

#### Mode d'étiquetage :

- `label [options] [input data] [output data]`
  - `--me`: Force le mode maxent
  - `--model FILE`: Fichier de modèle à charger
  - `--label`: Sortie uniquement les étiquettes
  - `--check`: L'entrée est déjà étiquetée
  - `--score`: Ajouter les scores à la sortie
  - `--post`: Étiqueter en utilisant les probabilités a posteriori
  - `--nbest INT`: Liste des n meilleures réponses
  - `--force`: Utiliser le décodage forcé

## Entraînement

Pour entraîner un CRF, vous avez besoin d'un fichier texte avec des annotations, où chaque ligne est un jeton avec ses étiquettes séparées par une tabulation.

Exemple :

```
UNITED STATES  NOUN  LOCATION_b
SECURITIES  NOUN  ORGANISATION_i
AND  CONJ  ORGANISATION_i
EXCHANGE  NOUN  ORGANISATION_i
COMMISSION  NOUN  ORGANISATION_i
Washington  NOUN  ORGANISATION_i
, PUNCT  ORGANISATION_i
D.C. NOUN  LOCATION_b
20549  DIG NUMBER_b
FORM  VERB  null
N NOUN  null
```

Dans cet exemple, chaque ligne représente un jeton associé à deux étiquettes différentes. L'étiquette "null" dans cet exemple est une simple chaîne de caractères qui n'a pas d'interprétation spécifique, sauf pour cet exemple spécifique.

Vous avez également besoin d'un fichier "pattern", qui doit être implémenté selon le manuel de CRF++ ou tel que décrit dans le [manuel de Wapiti](http://wapiti.limsi.fr/manual.html).

Exemple de fichier de motifs :

```
# Unigram
U00:%x[-2,0]
U01:%x[-1,0]
U02:%x[0,0]
U03:%x[1,0]
U04:%x[2,0]
U05:%x[-2,1]
U06:%x[-1,1]
U07:%x[0,1]
U08:%x[1,1]
U09:%x[2,1]
U10:%x[-2,0]/%x[0,0]
U11:%x[-1,0]/%x[0,0]
U12:%x[0,0]/%x[1,0]
U13:%x[0,0]/%x[2,0]
U14:%x[-2,1]/%x[0,1]
U15:%x[-1,1]/%x[0,1]
U16:%x[0,1]/%x[1,1]
U17:%x[0,1]/%x[2,1]
# Bigram
B
```

## Programme

Voici un petit programme qui utilise la bibliothèque pour entraîner un modèle et étiqueter des entités.

```python
use('wapiti')

wapiti tst

# Charger le modèle
tst.loadmodel("model")

# Étiqueter un vecteur de jetons
words = ['Growth NOUN', '& CONJ', 'Income NOUN', 'Fund NOUN', '( PUNCT', 'Exact ADJ', 'name NOUN', 'of PREP', 'registrant NOUN', 'as PREP', 'specified ADJ', 'in PREP', 'charter NOUN']
res = tst.label(words)

# Afficher la liste des étiquettes
println(res)
```

Le résultat est : `['ORGANISATION_b', 'ORGANISATION_i', 'ORGANISATION_i', 'ORGANISATION_i', 'null', 'null', 'null', 'null', 'null', 'null', 'null', 'null', 'null']`