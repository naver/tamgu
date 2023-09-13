## Type Dependency

A dependency is a relation between two synodes. Dependencies can be created directly through type dependency and stored in the knowledge base using `assertz` or with a dependency rule. A dependency consists of a name, a feature set, and a list of arguments.

### Methods

The `Dependency` class exposes the following methods:

1. `_initial([name, features, arg1, arg2, ...])`: Create a dependency with a name (string), a feature set (a map), and a list of arguments, each of type synode.
2. `features()`: Return the dependency features.
3. `name()`: Return the dependency name.
4. `rule()`: Return the rule id that created this dependency.

Example:

```java
// We create two lexical nodes
Synode det({"pos" :"det", "surface" :"the"});
Synode noun({"pos" :"noun", "surface" :"dog"});
Dependency d(["DET", {"direct" : "+"}, det, noun]);

// We add it to the knowledge base
assertz(d);
println(d);
```

Result: `DET['direct' : '+']({"pos" :"det", "surface" :"the"}, {"pos" :"noun", "surface" :"dog"})`

### Dependency Rule

A dependency rule matches the following pattern:

```
If ([^|~] dep[features] (#x[features], #y) and/or dep(#w, #z)...)
depres(#n, #nn), ..., depres(#n, #nn) / ~ / #x[..], #x[..]
```

Where `x`, `y`, `w`, `z`, `n`, `nn` are integers.
- Each of the `#x` are synodes that will be matched against the actual synodes in the knowledge base dependencies.
- A dependency can be preceded by `^` or `~`.
- A dependency name can also be replaced with `_n`, where `n` is an integer. You can compare dependencies or their names using these variables. If you use one of these variables in a dependency result, the name of the dependency recorded in the variable will be used to create this new dependency. `_n` can match any dependencies in memory as long as their arity matches.
- See below for a description of the feature structure.

**NOTE**: The `If` that starts such a rule should always start with a capital "I", otherwise, the system will try to parse the rule as an ordinary "if" Boolean expression.

The rule reads: If we have dependencies in the knowledge base that match against the one stored in the knowledge base, then we store some new dependencies using the same variable. The rule can mix function calls, predicates, and dependencies together.

The `^` means that this dependency will be modified. Only one dependency can be modified at a time in a rule.

The `~` is the negation. Before a dependency, it means that the dependency should not be present in the knowledge base.

If you replace the output of the rule with `~`, then the rule will apply, but no dependencies will be created.

### Fact

The simplest way to add a dependency to the knowledge base is to insert it as a fact.

```
dep[features](#1, #2).
```

### Features

The feature structure in a dependency rule follows specific rules:
- First, the quotes are optional around attributes and values.
- Second, the "+" is the default value of any attributes with one value.

**Operators**:
- `attribute`: Check the existence of the attribute.
- `attribute : value`: Compare the attribute against the value.
- `attribute : ~`: The attribute should not have any value.
- `attribute ~: value`: The attribute should not have the value.
- `attribute = value`: Give the attribute the value.
- `attribute = ~`: Remove the attribute from the feature set.
- `attribute -: gram`: Compare the attribute against a TREG (Tamgu Regular Expression).
- `attribute ~ -: gram`: Compare the attribute against a TREG, which should fail.

### Stop

It is also possible to stop the evaluation of a grammar with the predicate `stop`, as shown in the example below:

```
If (stop) ~.
```

This rule can be placed anywhere in the grammar. When the evaluation engine reaches this rule, it stops evaluating the grammar further. The next rules are then skipped altogether.

### _dependencies()

This method triggers a dependency analysis, applying rules against the knowledge base.

### _setvalidfeatures(mapss features)

This method puts constraints on the valid features that can be used for both synodes and dependencies. A feature is an attribute/value mapped over a key/value structure in the map. If an attribute can take any value, such as the lemma of a word, then the value should be an empty string. The default value is "+".

Example:

```java
mapss feats = {'Obl': '+', 'lemma': '', 'c_person': '+', 'CR4': '+', 'Punct': '+', 'surface': ''};
_setvalidfeatures(feats);
```