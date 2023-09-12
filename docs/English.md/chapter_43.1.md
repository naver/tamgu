## Inference Engine in Tamgu

Tamgu includes an inference engine that can be freely mixed with regular Tamgu instructions. This inference engine is similar to Prolog, but with some differences:

1. Predicates do not need to be declared beforehand in order for Tamgu to distinguish them from normal functions. However, if you plan to use a predicate that will be implemented later in the code, you need to declare it beforehand.

2. Inference variables do not need to be declared, but their names are different from traditional Prolog names. They must be preceded by a "?".

3. Each inference clause ends with a "." instead of a ";".

4. Terms can be declared beforehand as term variables. If you choose not to declare them, you must precede their name with a "?" as you would for inference variables.

5. Probabilities can be attached to predicates, allowing the engine to choose the path with the highest probability as the first option.

Note: For a proper description of the Prolog language, please consult the appropriate documentation.