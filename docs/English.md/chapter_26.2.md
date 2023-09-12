## Methods

1. `add(container, bool norm, int encoding)`: Transform a container (vector or map) into a transducer lexicon. If the container is a vector, it should have an even number of values.

2. `build(string input, string output, bool norm, int encoding)`: Build a transducer file from a text file. The text file should contain the surface form on the first line, followed by the lemma+features on the next line.

3. `compilergx(string rgx, svector features, string filename)`: Build a transducer file from regular expressions. The `filename` parameter is optional. The resulting automaton is stored in a file.

4. `load(string file)`: Load a transducer file.

5. `lookdown(string lemFeat, byte lemma)`: Search for a surface form that matches a lemma plus features. The `lemma` parameter is optional. If it is set to 1 or 2, the string to compare against can be reduced to only a lemma. If `lemma` is 2, features are also returned. Important: The lemma should be separated from the features with a tab.

6. `lookup(string wrd, int threshold, int flags)`: Look up a word using a transducer and a set of potential actions combined with a threshold. The last two arguments can be omitted. The potential actions are as follows:
   - `a_first`: The automaton can apply actions to the first character. If this action is not set, all the strings that will be compared against it will start with this character. If this character is not present in the automaton, the system will switch to this mode.
   - `a_change`: The automaton can change a character to another.
   - `a_delete`: The automaton can delete a character.
   - `a_insert`: The automaton can insert a character.
   - `a_switch`: The automaton transposes two characters.
   - `a_nocase`: The automaton checks if the two characters can match independently of their case.
   - `a_repetition`: The automaton accepts that a character is repeated a few times.
   - `a_vowel`: The automaton compares de-accentuated vowels together. For instance, "e" will match "é" or "è" but not "a".
   - `a_surface`: The automaton only returns surface form.

7. `parse(string sentence, int option, int threshold, int flags)`: Analyze a sequence of words using a transducer. The `option` parameter can take the following values:
   - `0`: Returns only the surface forms that were recognized within the initial string.
   - `1`: Same as `0`, but with their offsets.
   - `2`: Returns the surface forms and the lemmas with their features that were recognized in the initial input.
   - `3`: Same as `2`, but with their offsets. The `threshold` and `flags` are optional and follow the same convention as for `lookup`.

8. `store(string output, bool norm, int encoding)`: Store a transducer into a file. The last two parameters are optional.