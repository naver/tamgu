# Bibliothèque LINEAR : Type linéaire

Tamgu fournit une encapsulation de la bibliothèque liblinear, qui est utilisée pour implémenter des classificateurs. Pour plus d'informations sur cette bibliothèque, veuillez visiter [http://www.csie.ntu.edu.tw/~cjlin/liblinear/](http://www.csie.ntu.edu.tw/~cjlin/liblinear/).

## Méthodes

La bibliothèque expose les méthodes suivantes :

1. `cleandata()`: Nettoie les données internes.
2. `crossvalidation()`: Relance la validation croisée avec de nouveaux paramètres. Le résultat est une fmap.
3. `loadmodel(string filename)`: Charge un modèle. Un modèle peut également être chargé automatiquement avec un constructeur.
4. `options(smap actions)`: Définit les options d'entraînement (voir ci-dessous).
5. `predict(fvector labels, vector data, bool predict_probability, bool infos)`: Prédit à partir d'un vecteur de treemapif. Le paramètre `labels` est facultatif. Lorsqu'il est fourni, il est utilisé pour tester l'étiquette prédite par rapport à l'étiquette cible stockée dans `labels`. Si `infos` est vrai, le premier élément de ce vecteur est une carte d'informations.
6. `predictfromfile(string input, bool predict_probability, bool infos)`: Prédit à partir d'un fichier d'entrée. Le résultat est un vecteur. Si `infos` est vrai, le premier élément de ce vecteur est une carte d'informations.
7. `savemodel(string outputfilename)`: Sauvegarde le modèle dans un fichier.
8. `trainingset(fvector labels, vector data)`: Crée l'ensemble d'entraînement à partir d'un vecteur treemapif.
9. `train(string inputdata, smap options, string outputfilename)`: Entraîne les données d'entraînement avec certaines options. `outputfilename` est facultatif. Il sera utilisé pour stocker le modèle final s'il est fourni (la méthode peut également être appelée avec le nom `load`).

## Options d'entraînement

Les options d'entraînement doivent être fournies sous la forme d'une smap, avec les clés suivantes : `s`, `e`, `c`, `p`, `B`, `wi`, `M` et `v`.

1. `s` type: Définit le type de solveur (par défaut 1). Les valeurs possibles sont :
   - Pour la classification multiclasse :
     - 0 : Régression logistique régularisée L2 (primal)
     - 1 : Classification de vecteurs de support régularisée L2 avec perte L2 (dual)
     - 2 : Classification de vecteurs de support régularisée L2 avec perte L2 (primal)
     - 3 : Classification de vecteurs de support régularisée L2 avec perte L1 (dual)
     - 4 : Classification de vecteurs de support par Crammer et Singer
     - 5 : Classification de vecteurs de support régularisée L1 avec perte L2
     - 6 : Régression logistique régularisée L1
     - 7 : Régression logistique régularisée L2 (dual)
   - Pour la régression :
     - 11 : Régression de vecteurs de support régularisée L2 avec perte L2 (primal)
     - 12 : Régression de vecteurs de support régularisée L2 avec perte L2 (dual)
     - 13 : Régression de vecteurs de support régularisée L2 avec perte L1 (dual)
2. `c` cost: Définit le paramètre C (par défaut 1).
3. `p` epsilon: Définit l'epsilon dans la fonction de perte de SVR (par défaut 0.1).
4. `e` epsilon: Définit la tolérance du critère d'arrêt. Les valeurs dépendent du type `s` :
   - Pour `s` 0 et 2 : `|f'(w)|_2 <= eps*min(pos,neg)/l*|f'(w0)|_2`, où `f` est la fonction duale et `pos/neg` sont le nombre de données positives/négatives (par défaut 0.01).
   - Pour `s` 11 : `|f'(w)|_2 <= eps*|f'(w0)|_2` (par défaut 0.001).
   - Pour `s` 1, 3, 4 et 7 : Violation maximale duale <= eps ; similaire à libsvm (par défaut 0.1).
   - Pour `s` 5 et 6 : `|f'(w)|_1 <= eps*min(pos,neg)/l*|f'(w0)|_1`, où `f` est la fonction duale (par défaut 0.01).
   - Pour `s` 12 et 13 : `|f'(alpha)|_1 <= eps |f'(alpha0)|`, où `f` est la fonction duale (par défaut 0.1).
5. `B` bias: Si bias >= 0, l'instance x devient [x ; bias] ; si < 0, aucun terme de biais n'est ajouté (par défaut -1).
6. `wi` weight: Les poids ajustent le paramètre C des différentes classes. 'i' représente ici un indice. Une clé peut ressembler à "w10" par exemple.
7. `M` type: Définit le type de classification multiclasse (par défaut 0). Les valeurs possibles sont :
   - `M` 0 : un-contre-tous
   - `M` 1 : un-contre-un
8. `v` n: mode de validation croisée en n plis.

Notez qu'il est possible d'utiliser les chaînes de caractères suivantes à la place des entiers pour le solveur :
- "L2R_LR" est 0
- "L2R_L2LOSS_SVC_DUAL" est 1
- "L2R_L2LOSS_SVC" est 2
- "L2R_L1LOSS_SVC_DUAL" est 3
- "MCSVM_CS" est 4
- "L1R_L2LOSS_SVC" est 5
- "L1R_LR" est 6
- "L2R_LR_DUAL" est 7
- "L2R_L2LOSS_SVR = 11" est 8
- "L2R_L2LOSS_SVR_DUAL" est 9
- "L2R_L1LOSS_SVR_DUAL" est 10

## Exemples

### Exemple d'entraînement

```cpp
use("linear");
// Charger la bibliothèque
use("liblinear");

string trainFile = "output.dat";
// Déclarer une variable liblinear
liblinear train;
// Définir les options
map options = {"c": 100, "s": 'L2R_LR', "B": 1, "e": 0.01};
// Charger le modèle et stocker la sortie d'entraînement dans le fichier "model_test"
train.load(trainFile, options, "model_test");
```

### Exemple de prédiction

```cpp
use("linear");
// Le fichier d'entrée
string testFile = "trainData.dat";
// Déclarer une variable liblinear avec son modèle (on pourrait utiliser loadmodel à la place)
liblinear predict("model_test");
// Effectuer la prédiction à partir d'un fichier
vector result = predict.predictfromfile(testFile, true);
```