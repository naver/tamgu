# Tamgu : Un orchestrateur d'agents IA multi-paradigme

## Introduction

Tamgu est un langage de programmation innovant conçu pour orchestrer des agents d'Intelligence Artificielle (IA). Il fusionne de manière unique les paradigmes de programmation fonctionnelle, impérative et logique (FIL), offrant ainsi une plateforme puissante et flexible pour le développement d'applications IA avancées.

## Caractéristiques principales

### 1. Intégration transparente des modèles de langage (LLM)

Tamgu permet d'incorporer directement des appels à des LLM dans le flux de programmation. Cette intégration estompe la frontière traditionnelle entre la programmation classique et l'IA générative.

Exemple :
```prolog
vprompt("llama3", ?problem, ?query, ?generated_code)
```

Dans cet exemple :
- "llama3" est le nom du modèle LLM utilisé
- ?problem contient la description du problème à résoudre
- ?query est la question ou l'instruction spécifique pour le LLM
- ?generated_code est la variable qui recevra le code généré par le LLM

Cette fonction permet à Tamgu d'interagir directement avec le LLM pour générer du code ou des réponses basées sur le problème et la requête fournis.

### 2. Moteur de prédicats inspiré de Prolog

Le cœur logique de Tamgu s'inspire fortement de Prolog, offrant un puissant moteur de prédicats pour le raisonnement et la résolution de problèmes complexes.

Exemple :
```prolog
create(?P, ?Q, ?R):-
    ?model is "llama3",
    println("Model:", ?model),
    vprompt(?model, ?P, ?Q, ?Gen),
    extractcode(?Gen, ?code),
    execution(?model, ?code, ?R).
```
Ce prédicat `create` illustre le processus de création et d'exécution de code :
1. Il définit le modèle à utiliser ("llama3")
2. Affiche le nom du modèle utilisé
3. Utilise vprompt pour générer du code basé sur le problème ?P et la requête ?Q
4. Extrait le code Python de la réponse générée
5. Exécute le code extrait et stocke le résultat dans ?R

Cette structure permet à Tamgu de générer et d'exécuter du code de manière dynamique et adaptative.

### 3. Gestion multi-modèles

Tamgu peut orchestrer l'utilisation de plusieurs modèles d'IA, permettant de tirer parti des forces de différents LLM pour diverses tâches.

Exemple :
```prolog
create(?P, ?Q, ?R):-
    ?model is "command-r",
    // ... code ...
create(?P, ?Q, ?R):-
    ?model is "llama3",
    // ... code ...
create(?P, ?Q, ?R):-
    ?model is "llama3:70b",
    // ... code ...
```
Cette série de prédicats `create` montre comment Tamgu peut essayer différents modèles (command-r, llama3, llama3:70b) pour résoudre un problème. Le système tentera chaque modèle dans l'ordre jusqu'à ce qu'une solution satisfaisante soit trouvée, illustrant la capacité de Tamgu à s'adapter et à utiliser le modèle le plus approprié pour chaque tâche.

### 4. Génération et exécution de code dynamique

Tamgu peut générer, analyser et exécuter du code Python de manière dynamique, ouvrant la voie à des agents IA auto-adaptatifs.

Exemple :
```prolog
function execute_code(string c, ?_ v) {
    println("execute code");
    string py_err;
    try {
        v = p.run(c, "hotel_bill");
        return true;
    }
    catch(py_err) {
        v = py_err;
        return false;
    }
}
```
Cette fonction `execute_code` :
1. Prend une chaîne de caractères `c` contenant du code Python
2. Tente d'exécuter ce code en utilisant l'interpréteur Python intégré (`p.run`)
3. Si l'exécution réussit, elle stocke le résultat dans la variable `v` et retourne true
4. En cas d'erreur, elle capture l'erreur dans `py_err`, la stocke dans `v`, et retourne false

Cette approche permet à Tamgu d'exécuter du code généré dynamiquement tout en gérant les erreurs potentielles.

### 5. Gestion avancée des erreurs et auto-correction

Tamgu intègre des mécanismes sophistiqués pour détecter et corriger les erreurs dans le code généré, améliorant ainsi la robustesse des agents IA.

Exemple :
```prolog
execute_check(false, ?model, ?code, ?r, ?res) :-
    printlnerr("Correction"),
    ?c is ?r + "\n```python "+?code + "```",
    vprompt(?model, correction, ?c, ?Gen),
    extractcode(?Gen, ?codebis),
    execute_code_final(?codebis, ?res),
    println("Execution done after correction").
```
Ce prédicat `execute_check` est appelé lorsque l'exécution du code initial échoue. Il :
1. Affiche un message d'erreur
2. Prépare une chaîne contenant l'erreur et le code original
3. Utilise vprompt pour demander au modèle de corriger le code
4. Extrait le code corrigé
5. Tente d'exécuter le code corrigé
6. Affiche un message de confirmation si l'exécution réussit

Cette capacité d'auto-correction permet à Tamgu de gérer les erreurs de manière autonome et d'améliorer la fiabilité des agents IA.

## Fonctionnement de Tamgu comme orchestrateur d'agents

1. **Définition du problème** : L'utilisateur définit le problème et les questions à résoudre dans des chaînes de caractères structurées.

2. **Sélection du modèle** : Tamgu utilise une approche par backtracking pour essayer différents modèles LLM (command-r, llama3, llama3:70b) jusqu'à ce qu'une solution satisfaisante soit trouvée.

3. **Génération de code** : Le modèle sélectionné génère du code Python pour résoudre le problème spécifié.

4. **Extraction et exécution** : Tamgu extrait le code Python de la réponse formatée et tente de l'exécuter.

5. **Gestion des erreurs** : Si l'exécution échoue, Tamgu capture l'erreur et utilise le LLM pour corriger le code.

6. **Itération et amélioration** : Le processus se répète jusqu'à ce qu'une solution valide soit trouvée ou que toutes les options soient épuisées.

## Avantages de l'approche Tamgu

1. **Flexibilité** : Tamgu peut s'adapter à une grande variété de problèmes en utilisant différents modèles et stratégies.

2. **Robustesse** : La gestion avancée des erreurs et l'auto-correction améliorent la fiabilité des agents IA.

3. **Évolutivité** : Il est facile d'ajouter de nouveaux modèles ou de nouvelles stratégies de résolution de problèmes.

4. **Interopérabilité** : Tamgu peut interagir de manière transparente avec Python, permettant l'utilisation d'un vaste écosystème de bibliothèques.

5. **Puissance expressive** : La combinaison de paradigmes permet d'exprimer des logiques complexes de manière concise et efficace.

## Cas d'utilisation potentiels

- **Automatisation de tâches complexes** : Génération et exécution automatique de scripts pour des tâches variées.
- **Résolution de problèmes adaptatifs** : Création d'agents capables de s'adapter à des problèmes changeants.
- **Analyse et traitement de données avancés** : Combinaison de l'analyse logique et du traitement numérique.
- **Développement assisté par IA** : Aide à la génération et à la correction de code pour les développeurs.
- **Systèmes d'IA conversationnels avancés** : Création d'agents capables de raisonner et d'exécuter des actions basées sur des conversations.

## Conclusion

Tamgu représente une avancée significative dans le domaine de l'orchestration d'agents IA. En combinant la puissance de la programmation logique, la flexibilité de la génération de code dynamique, et l'intelligence des modèles de langage avancés, Tamgu ouvre la voie à une nouvelle génération d'applications IA plus adaptatives, robustes et puissantes.

Son approche unique permet de créer des agents IA capables non seulement de raisonner sur des problèmes complexes, mais aussi de générer et d'exécuter du code pour résoudre ces problèmes de manière autonome. Cette capacité d'auto-amélioration et d'adaptation fait de Tamgu un outil précieux pour relever les défis actuels et futurs de l'IA.
