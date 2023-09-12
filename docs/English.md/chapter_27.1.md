# Annotator in Tamgu

An annotator rule is a label associated with a body ending with a period (.):

```
(@|#|~)label <- e1,e2,e3.
```

Annotator rules are divided into four groups:

- Lexicon rules, which start with a "@"
- Annotation rules
- Global rules, which start with a "#", and apply to existing labels
- Delete rules, which start with a "~", and remove existing annotations

Rules are directly written as such in a Tamgu program. However, you need an "annotator" variable to access them.