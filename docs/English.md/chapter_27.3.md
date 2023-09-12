## Callback Function

You can associate a callback function with an annotator object.

**Note:** This callback function can only be used if lexicons have been provided.

The annotator callback function is called after the parsing has taken place, but before the rules are applied to the texts.

This callback function has the following signature:

```javascript
// Simply words associated with a "lexicon label"
function catching(vector v, annotator a) {
    v = pos.tagger(v);
    return v;
}
```

All these rules will be accessed through an annotator `r` with `catching`.

The vector `v` is a list of vectors, where each sub-vector contains:

`[word, lemma1, features1, lemma2, features2,.., leftOffset, rightOffset]`

The "word" is the token that was extracted from the text, while `lemma1`, `features1`, etc. are potential readings of this token. `leftOffset` and `rightOffset` are the positions in characters of that word within the text.

The returned vector is expected to have the same structure.

Example:

```javascript
['about', 'about', '+Adv+notly+ADV', 'about', '+Prep+PREP', '39', '44']
```

This function can be used to apply tagging, for instance, to the analysis beforehand.