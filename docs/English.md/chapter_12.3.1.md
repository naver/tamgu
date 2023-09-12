## String Handling

This section discusses various methods and implementations specific to strings.

### Korean Specific Methods (only for string and ustring)

1. `ishangul()`: Returns true if the character is a Hangul character.
2. `isjamo()`: Returns true if the character is a Hangul jamo.
3. `jamo(bool combine)`: If `combine` is false or absent, splits a Korean jamo into its main consonant and vowel components. If `combine` is true, combines the content into a jamo.
4. `normalizehangul()`: Normalizes different UTF8 encodings of Hangul characters.
5. `romanization()`: Romanization of Hangul characters.

### Latin Table

The Latin table is used in conjunction with the UTF8 method. The number associated with "Part" in the first column is used to access the encoding.

Latin -1 (Western European): This part covers most Western European languages such as Danish, Dutch, and English.

Note: Part 17 is an addendum to handle the "Windows 1252 Latin 1 (CP1252)" encoding.

### Encoding Names

To simplify referencing the right encoding, Tamgu provides the following constant values:

- `e_latin_we = 1`: Western European
- `e_latin_ce = 2`: Central European
- `e_latin_se = 3`: South European
- `e_latin_ne = 4`: North European
- `e_cyrillic = 5`: Cyrillic
- `e_arabic = 6`: Arabic
- `e_greek = 7`: Greek
- `e_hebrew = 8`: Hebrew
- `e_turkish = 9`: Turkish
- `e_nordic = 10`: Nordic
- `e_thai = 11`: Thai
- `e_baltic = 13`: Baltic Rim
- `e_celtic = 14`: Celtic
- `e_latin_ffe = 15`: Extended (French, Finnish, Estonian)
- `e_latin_see = 16`: South East European
- `e_windows = 17`: Windows encoding
- `e_cp1252 = 17`: Windows encoding (exact name match)

### Meta-characters

When using strings declared between double quotes (" "), Tamgu automatically recognizes the following meta-characters:

- `\\n`, `\\r`, and `\\t`: Line feed, carriage return, and tabulation respectively.

### Function `evaluate`

Tamgu also recognizes a large set of meta-characters when using the `evaluate` method. These meta-characters are automatically translated:

- Decimal code: `\\ddd`, which is translated into the Unicode character of that code. For example, `\\048` represents the character '0'.
- Hexadecimal code: `\\xhh`, which is also translated into the corresponding Unicode character.