# Dependency and Synode

Dependencies are a fundamental linguistic object that has become essential in modern Natural Language Processing. Tamgu offers a specific implementation of these dependencies based on the predicate engine. The goal of this implementation is to take the output of a dependency parser (such as the Stanford parser) as input and perform further analysis on it.

Dependencies are evaluated as predicates that connect syntactic nodes. Our system provides a second type called synode, which represents a node in a constituent tree.

## Synode

A synode is a syntactic node defined by a feature set (implemented as a mapss) and its position in the constituent tree. A synode has the following methods:

1. _initial(map m): Creates a syntactic node with the given features.
2. addchild(synode): Adds a child node.
3. addnext(synode): Adds a next node.
4. addprevious(synode): Adds a previous node.
5. after(synode): Returns true if the node is after the given node under the same parent.
6. attributes(): Returns the feature attributes as a vector.
7. before(synode): Returns true if the node is before the given node under the same parent.
8. child(): Returns the first child node or checks if it matches the parameter.
9. children(): Returns the list of children for a given node or checks if the node is a child.
10. definitions(mapss): Sets the valid feature definitions for all synodes.
11. last(): Returns the last child node or checks if it matches the parameter.
12. nbchildren(): Returns the number of direct children.
13. next(synode): Returns the next node or checks if it matches the parameter.
14. parent(): Returns the parent node or checks if it matches the parameter.
15. previous(synode): Returns the previous node or checks if it matches the parameter.
16. precede(synode): Returns true if the node is before (anywhere in the tree).
17. sibling(synode): Checks if the node is a sibling (either a sister or a descendant).
18. sisters(synode): Returns the list of sister nodes or checks if the node is a sister node.
19. succeed(synode): Returns true if the node is after (anywhere in the tree).
20. test(string attribute): Checks if an attribute is part of the feature structure.
21. values(): Returns the feature values as a vector.

## Creating a Constituent Tree

A constituent tree is built from top to bottom. When using the `addchild` function, it adds a child node under the current node, and each subsequent call to this function adds a new child after the previous child node.

Example:

```cpp
// We create our NP node
synode np({"pos": "np"});

// Then three lexical nodes
synode det({"pos": "det", "surface": "the"});
synode adj({"pos": "adj", "surface": "big"});
synode noun({"pos": "noun", "surface": "dog"});

// We add them under np, one after the other
np.addchild(det);
np.addchild(adj);
np.addchild(noun);

// We display the nodes in an indented way
function Display(synode x, int i) {
    if (x == null)
        return;
    string sp;
    sp.fill(i, " ");
    println(sp, x);
    Display(x.child(), i + 4);
    Display(x.next(), i);
}

Display(np, 0);
```

Result:
```
#0['pos': 'np']
    #0['pos': 'det', 'surface': 'the']
    #0['pos': 'adj', 'surface': 'big']
    #0['pos': 'noun', 'surface': 'dog']
```

Note the `#0`, which indicates that the synode is not a dependency variable.

## Type Dependency

A dependency is a relation between two synodes. Dependencies can be created directly using the `dependency` type, which can then be stored in the knowledge base with `assertz`, or with a dependency rule. A dependency consists of a name, a feature set, and a list of arguments.

Methods:
1. _initial([name, features, arg1, arg2, ...]): Creates a dependency with a name (string), a feature set (a mapss), and a list of arguments, each of type synode.
2. features(): Returns the dependency features.
3. name(): Returns the dependency name.
4. rule(): Returns the rule ID that created this dependency.

Example:

```cpp
// We create two lexical nodes
synode det({"pos": "det", "surface": "the"});
synode noun({"pos": "noun", "surface": "dog"});

dependency d(["DET", {"direct": "+"}, det, noun]);

// We add it to the knowledge base
assertz(d);

println(d);
```

Result:
```
DET['direct': '+']({"pos": "det", "surface": "the"}, {"pos": "noun", "surface": "dog"})
```

## Dependency Rule

A dependency rule follows the pattern:

```
If ([^|~] dep[features] (#x[features], #y) and/or dep(#w, #z)...)
    depres(#n, #nn), ..., depres(#n, #nn) / ~ / #x[..], #x[..].
```

In this rule, `x`, `y`, `w`, `z`, `n`, `nn` are integers that represent synode objects. The rule matches dependencies in the knowledge base against the ones stored in the rule. If a match is found, new dependencies are created using the same variables.

The rule can mix function calls and predicates with dependencies. The `^` symbol indicates that the dependency will be modified. Only one dependency can be modified at a time in a rule. The `~` symbol represents negation. If placed before a dependency, it means that the dependency should not be present in the knowledge base.

If the output of the rule is replaced with `~`, the rule will apply, but no dependencies will be created.

Fact:
The simplest way to add a dependency to the knowledge base is to insert it as a fact:

```
dep[features](#1, #2).
```

## Features

The feature structure in a dependency rule follows specific rules:
- Quotes are optional around attributes and values.
- The `+` symbol is the default value for attributes with one value.

Operators:
- `attribute`: Checks the existence of the attribute.
- `attribute: value`: Compares the attribute against the value.
- `attribute: ~`: The attribute should not have any value.
- `attribute ~: value`: The attribute should not have the specified value.
- `attribute = value`: Assigns the attribute the specified value.
- `attribute = ~`: Removes the attribute from the feature set.
- `attribute -: gram`: Compares the attribute against a TREG (Tamgu Regular Expression).
- `attribute ~ -: gram`: Compares the attribute against a TREG, which should fail.

Stop:
The predicate `stop` can be used to stop the evaluation of a grammar. When the evaluation engine encounters this predicate, it stops evaluating the grammar further, skipping the remaining rules.

```cpp
If (stop) ~.
```

_dependencies():
This method triggers a dependency analysis by applying rules against the knowledge base.

_setvalidfeatures(mapss features):
This method sets constraints on the valid features that can be used for synodes and dependencies. The features are defined as attribute/value pairs in a map. If an attribute can take any value, such as the lemma of a word, the value should be an empty string. The default value is `+`.

Example:

```cpp
mapss feats = {'Obl': '+', 'lemma': '', 'c_person': '+', 'CR4': '+', 'Punct': '+', 'surface': ''};
_setvalidfeatures(feats);
```

## Example

```cpp
// We display the nodes in an indented way
function Display(synode x, int i) {
    if (x == null)
        return;
    string sp;
    sp.fill(i, " ");
    println(sp, x);
    Display(x.child(), i + 5);
    if (i) // when i==0, it is the root of our tree, we do not want to display its sisters
        Display(x.next(), i);
}

// We prepare our constituent tree
synode np1 = {"bar": 2};
synode np2({"bar": 2});
synode vp({"bar": 2});
synode s({"bar": 3});
synode v({"word": "eats", "pers": 3, "pres": "+", "verb": "+"});
synode d1({"word": "the", "det": "+"});
synode n1({"word": "dog", "noun": "+"});
synode d2({"word": "a", "det": "+"});
synode n2({"word": "bone", "noun": "+"});

s.addchild(np1);
s.addchild(vp);
vp.addchild(v, np2);
np1.addchild(d1, n1);
np2.addchild(d2, n2);

// It is possible to add or modify existing features as if a synode was a map
vp["pos"] = "verb";
np1["pos"] = "noun";
np2["pos"] = "noun";

// Our initial dependencies
subj(v, n1).
obj(v, n2).
det(n1, d1).
det(n2, d2);

// We can also create it in a different way
dependency dpe(["mydep", {}, n1, n2]);

// But then we have to add it to the knowledge base ourselves
assertz(dpe);

// This function is called from a rule below. The #x becomes a synode.
// The function returns true to avoid the rule failing.
function DTree(synode n) {
    Display(n, 0);
    println("---------------------------");
    return true;
}

// A simple rule that inverts the nodes
If (subj(#1, #2)) inverted(#2, #1).

// A rule that uses constraints on nodes
If (subj(#1[pres, pers: 3], #2) and obj(#1, #3)) arguments(#2, #1, #3).

// We add features to a dependency
If (^subj(#1, #2)) subj[direct=+](#1, #2).

// We use _ to browse among all dependencies with two arguments, with a constraint that two nodes are different
If (_(#1, #2) && obj(#1, #3) && #2 != #3) link(#2, #3).

// We use dependency variables _1 and _2 to avoid creating a dependency between the same arguments
If (_1(#1, #2) && obj_2(#1, #3) && _1 != _2) other(#2, #3).

// We mark a node through a dependency rule, and we can use constraints in the structure as well
If (subj(#1, #2) and obj(#1, #3) and #2[noun:+, subject=+]) ~.

// We can also write this rule using quotes
If (subj(#1, #2) and obj(#1, #3) and #3["object"] = "+") ~.

// In this case, we access the parent of the node #1 using the p_parent predicate
// Then we call DTree to display it... DTree must return true, otherwise the rule will fail
// The #3 is automatically transformed into a synode object when the function is called
If (det(#1, #2) and p_parent(#1, #3) and DTree(#3)) ~.

// We use a TREG as a constraint in our rule
If (obj(#1[word -: "e%a+"], #2)) Verb(#1).

// We launch our dependency parser
_dependencies();

// We gather all the dependencies in the knowledge base
vector res = predicatedump();

Display(s, 0);
println("---------------------------");
printjln(res);
```

Result:
```
s
#0['bar': '2', 'pos': 'noun']
    #0['word': 'the', 'det': '+']
    #0['word': 'dog', 'noun': '+', 'subject': '+']
---------------------------
#0['bar': '2', 'pos': 'noun']
    #0['word': 'a', 'det': '+']
    #0['object': '+', 'word': 'bone', 'noun': '+']
---------------------------
#0['bar': '3']
#0['bar': '2', 'pos': 'noun']
    #0['word': 'the', 'det': '+']
    #0['word': 'dog', 'noun': '+', 'subject': '+']
#0['bar': '2', 'pos': 'verb']
    #0['word': 'eats', 'pers': '3', 'pres': '+', 'verb': '+']
#0['bar': '2', 'pos': 'noun']
    #0['word': 'a', 'det': '+']
    #0['object': '+', 'word': 'bone', 'noun': '+']
---------------------------
other({"word": "dog", "noun": "+", "subject": "+"}, {"object": "+", "word": "bone", "noun": "+"})
subj['direct': '+']({"word": "eats", "pers": "3", "pres": "+", "verb": "+"}, {"word": "dog", "noun": "+", "subject": "+"})
inverted({"word": "dog", "noun": "+", "subject": "+"}, {"word": "eats", "pers": "3", "pres": "+", "verb": "+"})
obj({"word": "eats", "pers": "3", "pres": "+", "verb": "+"}, {"object": "+", "word": "bone", "noun": "+"})
det({"word": "dog", "noun": "+", "subject": "+"}, {"word": "the", "det": "+"})
det({"object": "+", "word": "bone", "noun": "+"}, {"word": "a", "det": "+"})
arguments({"word": "dog", "noun": "+", "subject": "+"}, {"word": "eats", "pers": "3", "pres": "+", "verb": "+"}, {"object": "+", "word": "bone", "noun": "+"})
link({"word": "dog", "noun": "+", "subject": "+"}, {"object": "+", "word": "bone", "noun": "+"})
Verb({"word": "eats", "pers": "3", "pres": "+", "verb": "+"})
```